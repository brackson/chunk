CFLAGS = -g -Wall
CC = gcc

# if make followed by no target then will compile 'Hello'
all: chunk

# before the "rm" there is only one white space character: a "TAB".
# If you have any  other white space this Makefile while not work

# before the "${CC}" there is only one white space character: a "TAB".
# If you have any  other white space this makefile while not work

chunk: chunk.c fileWriter.c
	$(CC) $(CFLAGS) $^ -o $@

# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r *.exe
