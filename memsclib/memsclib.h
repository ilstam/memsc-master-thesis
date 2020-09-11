#ifndef MEMSCLIB_H
#define MEMSCLIB_H

#include <stdbool.h>

enum {
	MEMSCL_ERR_NOMEM = -200, /* memory allocation failed */
	MEMSCL_ERR_ALREADY_REG,  /* this process is already registered */
	MEMSCL_ERR_CLONE,        /* couldn't spawn a new thread */
	MEMSCL_ERR_REG_FAILED,   /* the memsc_register syscall failed */
	MEMSCL_ERR_NOSLOT,       /* no system call slot was free */
	MEMSCL_ERR_IDX_OOB,      /* the index provided is out of bounds */
};

typedef int memsc_idx;


/* Register for memsc services. It should always be called first.
 * Returns 0 on success, non-zero otherwise. */
int memsc_register(void);
/* Post a new syscall. */
memsc_idx memsc_add(int sysno, ...);
/* Returns true if the given entry has been processed, else false. */
bool memsc_ready(memsc_idx idx);
/* Block until any of the posted syscalls have been processed. */
void memsc_wait_all(void); 
/* Block until all of the posted syscalls have been processed. - Not implemented. */
void memsc_wait_any(void); 
/* Return the system call result.
 * This function frees the entry and cannot be called twice. */
long memsc_retval(memsc_idx idx);

#endif
