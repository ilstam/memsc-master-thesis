#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>

#include "memsclib.h"


int main()
{
	int fd;
	char buf[2001] = {0};

	if (memsc_register())
		exit(-1);

	/* synchronous syscall */
	fd = open("/etc/fstab", 0, "r");

	/* asynchronous syscalls */
	memsc_idx m_getpid = memsc_add(__NR_getpid);
	memsc_idx m_mkdirat = memsc_add(__NR_mkdirat, AT_FDCWD, "mydir", 0);
	memsc_idx m_read = memsc_add(__NR_read, fd, buf, 2000);

	while (!memsc_ready(m_getpid))
		;

	printf("getpid done: %ld\n", memsc_retval(m_getpid));

	while (!memsc_ready(m_mkdirat))
		;

	printf("mkdirat done: %ld\n", memsc_retval(m_mkdirat));

	while (!memsc_ready(m_read))
		;

	long r = memsc_retval(m_read);
	printf("read done: %ld\n", r);
	if (r > 0)
		puts(buf);

	return 0;
}
