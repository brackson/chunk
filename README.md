# chunk Command

## Usage
```
chunk [options] -f filename.txt [-p prefix] [-s suffix]
chunk [options] [-p prefix] < filename.txt
```

## Options
```
-f filename
        The file you want to use as input.
-l line_count
        (default, 1000) Split file into x-line files. Marks new lines
        by searching for newline characters ("/n").
-w word_count
        Split file into x-word files. Marks new words by searching for
        whitespace characters.
-c character_count
        Split file into x-character files. Marks characters by
        searching for non-whitespace characters.
```

If you do not specify a file, the program will let you type in the terminal and
process it using your commands. Exit with Ctrl-Z + Enter.

Included is a 2048-byte lorem.txt file (for testing purposes).

## Summary
Chunk was written in C for a Systems Programming assignment. It splits files by either lines, words, or characters. It accepts prefixes and suffixes. Its default prefix is "x" and its default suffix is "aa, ab, ac, ... zz".

You can use the -p flag to specify a custom prefix. You can use the -s flag to specify a custom suffix, making the suffix numeric instead of alphabetical. It defaults to zero but you can set it to whatever number you want to begin from. Using numeric suffixes do not have a cap, but alphabetical ones do (650). This is because there are only 650 combinations of aa...zz.
