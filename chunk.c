#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 128
#include "fileWriter.h"

/*GLOBALS */
int line_count = 1000;
int word_count = 0;
int character_count = 0;
char *filename;
char *prefix;

int suffix_type = 1; // 0=numeric, 1=alphabet (default)

char suffix_alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
int suffix_first_index = 0;
int suffix_second_index = 0;

mode_t input_file_mode;

char output_file_name[100];
int output_file_count = 0;
int output_fd;
int total_bytes_saved = 0;

enum chunk_types {Line, Word, Character};
enum chunk_types chunk_type = Line;

int main(int argc, char **argv) {
	int index;
	int c;

	opterr = 0;

	while ((c = getopt(argc, argv, "f:l:w:c:p:s:")) != EOF) {
		switch (c) {
			case 'l':
				line_count = atoi(optarg);
				chunk_type = Line;
				break;
			case 'w':
				word_count = atoi(optarg);
				chunk_type = Word;
				break;
			case 'c':
				character_count = atoi(optarg);
				chunk_type = Character;
				break;
			case 'f':
				filename = optarg;
				break;
            case 'p':
                prefix = optarg;
                break;
            case 's':
                suffix_type = 0;  // 0=numeric, 1=alphabet (default)
                output_file_count = atoi(optarg);
                break;
			case '?':
				if (optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
				return 1;
			default:
				abort();
		}
    }

    for (index = optind; index < argc; index++) {
        printf("Non-option argument %s\n", argv[index]);
        exit(1);
    }

    int input_fd;

    // if -f flag given, set input_fd to filename
    if (filename != NULL) {
        input_fd = open(filename, O_RDONLY);

        if (input_fd < 0) {
            fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
            exit(1);
        }

    } else {
        // set input_fd to STDIN_FILENO
        input_fd = STDIN_FILENO;
    }

    // get permissions of input_fd
    struct stat *inputstat = malloc(sizeof(struct stat));
    fstat(input_fd, inputstat);
    input_file_mode = inputstat->st_mode;

    // GENERATE output_file_name
    // if prefix is null, they didn't specify one with the -p flag.
    if (prefix == NULL) {
        if (suffix_type == 1) { // default prefix, alphabet suffix
            sprintf(output_file_name, "x%c%c", suffix_alphabet[suffix_first_index], suffix_alphabet[suffix_second_index]);
        } else {  // default prefix, default (numeric) suffix
            sprintf(output_file_name, "x%.2d", output_file_count);
        }
    } else {
        if (suffix_type == 1) { // custom prefix, alphabet suffix
            sprintf(output_file_name, "%s%c%c", prefix, suffix_alphabet[suffix_first_index], suffix_alphabet[suffix_second_index]);
        } else { // custom prefix, default (numeric) suffix
            sprintf(output_file_name, "%s%.2d", prefix, output_file_count);
        }
    }

    suffix_second_index += 1;

    // open first output_fd with specified output_file_name (made earlier)
    output_fd = open(output_file_name, O_WRONLY | O_CREAT | O_EXCL, input_file_mode);

    // if there was an error, print it and exit
    if (output_fd < 0) {
        fprintf(stderr, "Error opening file %s: %s\n", output_file_name, strerror(errno));
        exit(1);
    }

    char read_buff[BUFFER_SIZE];
    char write_buff[BUFFER_SIZE];
    int write_buff_size = 0;
    int temp_count = 0;
    int bytes;

    // loop over bytes of BUFFER_SIZE, put it in read_buff
    while((bytes = read(input_fd, read_buff, sizeof read_buff)) != 0) {

        switch (chunk_type) {
            case Line:
                // loop over each byte
                for (int i = 0; i < bytes; i++) {
                    // put byte in write_buff
                    write_buff[write_buff_size] = read_buff[i];
                    write_buff_size += 1;
                    total_bytes_saved += 1; // used for fileWriter.c

                    // detect newline, temp_count represents lines counted
                    if(read_buff[i] == '\n') {
                        temp_count += 1;
                    }

                    // if specified lines are counted, write write_buff to file
                    if (temp_count == line_count) {
                        writeToFile(write_buff, write_buff_size, 1);
                        memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                        temp_count = 0; // reset line count
                        write_buff_size = 0; // reset write_buff_size
                    }
                }

                // if after all bytes are looped over, there are things left in write_buff,
                // write them to file and reset write_buff
                if(write_buff_size > 0) {
                    writeToFile(write_buff, write_buff_size, 0);
                    write_buff_size = 0; // reset write_buff_size
                    memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                }

                break;

            case Word:
                // loop over each byte
                for (int i = 0; i < bytes; i++) {
                    // put byte in write_buff
                    write_buff[write_buff_size] = read_buff[i];
                    write_buff_size += 1;
                    total_bytes_saved += 1;

                    // detect space, temp_count represents word count
                    if(isspace(read_buff[i])) {
                        temp_count += 1;
                    }

                    // if specified words are counted, write write_buff to file
                    if (temp_count == word_count) {
                        writeToFile(write_buff, write_buff_size, 1);
                        memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                        temp_count = 0; // reset line count
                        write_buff_size = 0; // reset write_buff_size
                    }
                }

                // if after all bytes are looped over, there are things left in write_buff,
                // write them to file and reset write_buff
                if(write_buff_size > 0) {
                    writeToFile(write_buff, write_buff_size, 0);
                    write_buff_size = 0; // reset write_buff_size
                    memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                }

                break;

            case Character:
                // loop over each byte
                for (int i = 0; i < bytes; i++) {
                    // put byte in write_buff
                    write_buff[write_buff_size] = read_buff[i];
                    write_buff_size += 1;
                    total_bytes_saved += 1;

                    temp_count += 1;

                    // if specified chars are counted, write write_buff to file
                    if (temp_count == character_count) {
                        writeToFile(write_buff, write_buff_size, 1);
                        memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                        temp_count = 0; // reset line count
                        write_buff_size = 0;// reset write_buff_size
                    }
                }

                // if after all bytes are looped over, there are things left in write_buff,
                // write them to file and reset write_buff
                if(write_buff_size > 0) {
                    writeToFile(write_buff, write_buff_size, 0);
                    write_buff_size = 0; // reset write_buff_size
                    memset(write_buff, 0, sizeof write_buff); // wipe write_buff
                }

                break;
        }

    }

	return 0;
}
