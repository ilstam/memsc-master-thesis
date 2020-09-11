#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <stdatomic.h>

#include "memsc.h"
#include "memsclib.h"

#if defined(__aarch64__) || defined(__arm__)
#define __NR_memsc_register 439
#endif

/* Index of the next available entry in the ring buffer. */
static memsc_idx sysp_pos = 0;

/* This depends on the page size so it can only be determined during runtime. */
static int MEMSC_MAX_ENTRIES;

/*
 * The following variable points to the syscall page, a shared memory page
 * used for communication with the kernel. The memory it points to can be
 * modified asynchronously and concurrently by the kernel so we need to be
 * very careful with regards to memory ordering when accessing it.
 */
static struct memsc_sysentry *memsc_syspage;


/*
 * The memsc_register syscall invoked by this function will keep the thread
 * trapped in kernel mode in order to execute incoming system calls. This
 * thread will only return back to userspace when it receives a fatal signal.
 */
static int memsc_clone(__attribute__((unused)) void *arg) {
	long ret = syscall(__NR_memsc_register, memsc_syspage);

	/* if the syscall returned an error, let the parent thread know */
	if (ret < 0)
		memsc_syspage->sysret = ret;

	return 0;
}

/*
 * Registration is a 2-step procedure. First, we need to allocate a memory
 * page in userspace, which will be shared with the kernel, for posting system
 * calls. Second, we need to spawn a new thread that will be converted to a
 * memsc worker by invoking the memsc_register system call.
 */
int memsc_register(void)
{
	size_t pgsize;
	char *stack;
	int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
	                   CLONE_THREAD | CLONE_UNTRACED | 0);

	if (memsc_syspage)
		return MEMSCL_ERR_ALREADY_REG;

	pgsize = sysconf(_SC_PAGESIZE);
	MEMSC_MAX_ENTRIES = pgsize / sizeof(struct memsc_sysentry);

	/* the sysypage MUST be aligned to a page boundary */
	memsc_syspage = aligned_alloc(pgsize, pgsize);
	stack = malloc(pgsize);

	if (!memsc_syspage || !stack)
		return MEMSCL_ERR_NOMEM;

	/* zero out the new syscall page */
	memset(memsc_syspage, 0, pgsize);

	/* prepare to wait until the new thread changes this variable */
	memsc_syspage->sysret = MEMSC_REG_WAIT;

	if (clone(memsc_clone, stack + pgsize, clone_flags, NULL) == -1)
		return MEMSCL_ERR_CLONE;

	while (atomic_load_explicit(&memsc_syspage->sysret, memory_order_acquire)
	       == MEMSC_REG_WAIT)
		/* busy wait */;

	if (memsc_syspage->sysret != MEMSC_REG_DONE) {
		free(memsc_syspage);
		free(stack);
		return MEMSCL_ERR_REG_FAILED;
	}

	return 0;
}

memsc_idx memsc_add(int sysno, ...)
{
	va_list args;
	struct memsc_sysentry *entry = memsc_syspage + sysp_pos;

	if (entry->status != MEMSC_STATUS_FREE)
		return MEMSCL_ERR_NOSLOT;

	memset(entry, 0, sizeof(struct memsc_sysentry));
	entry->sysnum = sysno;

	va_start(args, 6);
	for (int i = 0; i < 6; i++)
		entry->args[i] = va_arg(args, long);
	va_end(args);

	if (++sysp_pos == MEMSC_MAX_ENTRIES)
		sysp_pos = 0;

	/* make sure the new status field becomes globally visible last in
	 * order to prevent the kernel from processing incomplete entries */
	atomic_store_explicit(&entry->status, MEMSC_STATUS_SUBMITTED,
	                      memory_order_release);

	return entry - memsc_syspage;
}

bool memsc_ready(memsc_idx idx) {
	struct memsc_sysentry *entry;
	memsc_status status;

	if (idx < 0 || idx >= MEMSC_MAX_ENTRIES)
		return MEMSCL_ERR_IDX_OOB;

	entry = memsc_syspage + idx;
	status = atomic_load_explicit(&entry->status, memory_order_acquire);

	return status == MEMSC_STATUS_DONE;
}

void memsc_wait_all(void)
{
	struct memsc_sysentry *entry;

	/* waiting for the last posted entry is enough since the kernel
	 * processes the entries in-order */
	if (sysp_pos == 0)
		entry = memsc_syspage + MEMSC_MAX_ENTRIES - 1;
	else
		entry = memsc_syspage + sysp_pos - 1;

	/* use a sw barrier to ensure the result is not cached in a register */
	while (entry->status == MEMSC_STATUS_SUBMITTED)
		__asm__ __volatile__("": : :"memory");
}

void memsc_wait_any(void)
{
	/* not implemented */
}

long memsc_retval(memsc_idx idx)
{
	long ret;
	struct memsc_sysentry *entry;

	if (idx < 0 || idx >= MEMSC_MAX_ENTRIES)
		return MEMSCL_ERR_IDX_OOB;

	entry = memsc_syspage + idx;
	ret = entry->sysret;

	/* make sure the return value is loaded before the status changes */
	atomic_store_explicit(&entry->status, MEMSC_STATUS_FREE,
	                      memory_order_release);

	return ret;
}
