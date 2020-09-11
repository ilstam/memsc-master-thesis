MEMSC_HEADER=/home/ilstam/l4new/src/l4linux/include/uapi/memsc/

CC=arm-buildroot-linux-uclibcgnueabihf-gcc
LD=arm-buildroot-linux-uclibcgnueabihf-ld

CFLAGS=-Wall -W -pedantic -D_GNU_SOURCE -Wno-varargs -c -I$(MEMSC_HEADER)

all: libmemsc.a libmemsc.so

libmemsc.a: memsclib.o
	ar crs $@ $+

libmemsc.so: memsclib.s.o
	$(LD) -shared -o $@ $+

memsclib.o: memsclib.c memsclib.h
	$(CC) $(CFLAGS) memsclib.c

memsclib.s.o: memsclib.c memsclib.h
	$(CC) $(CFLAGS) -fPIC -o $@ memsclib.c

clean:
	rm -f *.o *.a *.so
