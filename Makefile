CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic

all: lab1 remove

lab1: create.c
	$(CC) $(CFLAGS) create.c -o lab1

remove: remove.c
	$(CC) $(CFLAGS) remove.c -o remove

run: lab1
	./lab1 result

run-sh:
	./create.sh result

show:
	ls -laR result

remove-result: remove
	./remove result

clean:
	rm -f lab1 remove

.PHONY: all run run-sh show remove-result clean
