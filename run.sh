gcc src/main.c src/tokenizer.c src/parser.c src/exec.c -w -o icc
# there are a bunch of warnings between structs and their typedef equivalents, so we'll just disable warnings altogether
./icc code/main.i
