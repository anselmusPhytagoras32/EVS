CC = gcc
FLAGS = -Wall -lncurses -I lib -g
SRC = source/main.c source/display.c source/input.c source/rooms.c source/simulation.c source/size_checker.c

test: $(SRC)
	$(CC) -o test $(SRC) $(FLAGS)

.PHONY: clean_test

clean_test:
	rm test test.exe
