
PRJ=c401
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: $(PRJ).c $(PRJ)-test.c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c $(PRJ)-test.c


main: c401.c main.c
	$(CC) $(CFLAGS) -o main c401.c main.c

clean:
	rm -f *.o *.out $(PROGS)
#
