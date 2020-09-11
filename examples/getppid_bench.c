#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "memsclib.h"
#include "bench.h"

long num_syscalls = 10000;


void bench_ipi(uint64_t *before, uint64_t *after)
{
	READ_CNTR(*before);
	for (long i = 0; i < num_syscalls; i++)
		getppid();

	READ_CNTR(*after);
}

void bench_each(uint64_t *before, uint64_t *after)
{
	READ_CNTR(*before);
	for (long i = 0; i < num_syscalls; i++) {
		memsc_idx id = memsc_add(__NR_getppid);

		memsc_wait_all();
		memsc_retval(id);
	}
	READ_CNTR(*after);
}

void bench_all(uint64_t *before, uint64_t *after, int batch_size)
{
	memsc_idx list[batch_size];

	READ_CNTR(*before);
	for (long i = 0; i < num_syscalls; i++) {
		list[i%batch_size] = memsc_add(__NR_getppid);

		/* wait & free the slots in the syspage */
		if ((i+1)%batch_size == 0) {
			memsc_wait_all();
			for (int j = 0; j <= (i%batch_size); j++)
				memsc_retval(list[j]);
		}
	}

	memsc_wait_all();
	READ_CNTR(*after);
}

int main(int argc, char *argv[])
{
	uint64_t before, after;
	int batch_size;

	if (argc < 2 || argc > 4) {
		puts("usage: bench ipi|each|all [num_syscalls] [batch_size]");
		exit(-1);
	}

	if (argc >= 3)
		sscanf(argv[2], "%ld", &num_syscalls);

	printf("number of syscalls: %ld\n", num_syscalls);

	if (!strcmp(argv[1], "ipi")) {
		bench_ipi(&before, &after);
	} else if (!strcmp(argv[1], "each")) {
		if (memsc_register())
			exit(-3);
		bench_each(&before, &after);
	} else if (!strcmp(argv[1], "all")) {
		if (memsc_register())
			exit(-3);

		if (argc == 4)
			sscanf(argv[3], "%d", &batch_size);
		else
			batch_size = 64;

		printf("doing batches of %d\n", batch_size);
		bench_all(&before, &after, batch_size);
	} else {
		puts("usage: bench ipi|each|all [num_syscalls] [batch_size]");
		exit(-1);
	}

	printf("microsec: %llu\n", TO_MICROSEC(before, after));
	printf("cycles: %llu\n", TO_CYCLES(before, after));

	return 0;
}
