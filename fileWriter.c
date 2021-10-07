#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


extern char *filename;
extern char *prefix;

extern int suffix_type; // 0=numeric, 1=alphabet

extern char suffix_alphabet[26];
extern int suffix_first_index;
extern int suffix_second_index;

extern mode_t input_file_mode;

extern char output_file_name[100];
extern int output_file_count;
extern int output_fd;
extern int total_bytes_saved;

// create_new_file = 0 means only write, don't create a new output_fd
// create_new_file = 1 means write AND afterwards, create a new output_fd
int writeToFile(char write_buff[], int write_buff_size, int create_new_file) {
    write(output_fd, write_buff, write_buff_size);

    if(create_new_file) {
        output_file_count += 1;

        // GENERATE output_file_name
        // if prefix is null, they didn't specify one with the -p flag.
        if (prefix == NULL) {
            if (suffix_type == 1) { // default prefix, alphabet suffix
                sprintf(output_file_name, "x%c%c", suffix_alphabet[suffix_first_index], suffix_alphabet[suffix_second_index]);
            } else { // default prefix, default (numeric) suffix
                sprintf(output_file_name, "x%.2d", output_file_count);
            }
        } else {
            if (suffix_type == 1) { // custom prefix, alphabet suffix
                sprintf(output_file_name, "%s%c%c", prefix, suffix_alphabet[suffix_first_index], suffix_alphabet[suffix_second_index]);
            } else { // custom prefix, default (numeric) suffix
                sprintf(output_file_name, "%s%.2d", prefix, output_file_count);
            }
        }

        if(suffix_type == 1) { // alphabet; increment indexes
            if(suffix_second_index == 25) {
                suffix_first_index += 1;
                suffix_second_index = 0;
            } else {
                suffix_second_index += 1;
            }

            if(suffix_first_index == 25 && suffix_second_index == 25) {
                fprintf(stderr, "Max output reached. Made 650 files with %d bytes.", total_bytes_saved);
                exit(1);
            }
        }

        output_fd = open(output_file_name, O_WRONLY | O_CREAT | O_EXCL, input_file_mode);

        if (output_fd < 0) {
            fprintf(stderr, "Error opening file %s: %s\n", output_file_name, strerror(errno));
            exit(1);
        }

    }
    return 0;
}
