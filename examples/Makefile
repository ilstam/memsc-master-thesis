MEMSCLIB_DIR=/home/ilstam/memsclib/

CC=arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS=-Wall -W -pedantic -c -I$(MEMSCLIB_DIR)

all: example getppid_bench

example: example.o
	$(CC) -o $@ example.o -L$(MEMSCLIB_DIR) -lmemsc

example.o: example.c
	$(CC) $(CFLAGS) example.c

getppid_bench: getppid_bench.o
	$(CC) -o $@ getppid_bench.o -L$(MEMSCLIB_DIR) -lmemsc

getppid_bench.o: getppid_bench.c bench.h
	$(CC) $(CFLAGS) getppid_bench.c

clean:
	rm -f *.o example getppid_bench
