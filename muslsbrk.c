#include <syscall.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

void *muslsbrk(intptr_t inc)
{
	unsigned long cur = syscall(SYS_brk, 0);
	if (inc && syscall(SYS_brk, cur+inc) != cur+inc) {
		errno = ENOMEM;
		return (void *)-1;
	}
	return (void *)cur;
}
