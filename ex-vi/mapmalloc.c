/*
 * Memory allocation routines using mmap().
 *
 * Derived from mm-1.1.3 by Gunnar Ritter, Freiburg i. Br., Germany,
 * September 2001.
 */

/*	Sccsid @(#)mapmalloc.c	1.14 (gritter) 11/26/04	*/

/* ====================================================================
 * Copyright (c) 1999-2000 Ralf S. Engelschall. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by
 *     Ralf S. Engelschall <rse@engelschall.com>."
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by
 *     Ralf S. Engelschall <rse@engelschall.com>."
 *
 * THIS SOFTWARE IS PROVIDED BY RALF S. ENGELSCHALL ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL RALF S. ENGELSCHALL OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#ifdef	__GLIBC__
/*
 * Broken GNU libc will include stdlib.h with conflicting
 * malloc() types otherwise.
 */
#ifndef	__NO_STRING_INLINES
#define	__NO_STRING_INLINES
#endif	/* !__NO_STRING_INLINES */
#endif	/* __GLIBC__ */
#include <string.h>

#include "config.h"

#ifdef	LANGMSG
#include <nl_types.h>
extern	nl_catd	catd;
#else
#define	catgets(a, b, c, d)	(d)
#endif

#ifndef	min_of
#define	min_of(a, b)	((a) < (b) ? (a) : (b))
#endif
#ifndef	max_of
#define	max_of(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef	MAP_ANON
#ifdef	MAP_ANONYMOUS
#define MAP_ANON	MAP_ANONYMOUS
#endif
#endif

#ifndef	MAP_FAILED
#define MAP_FAILED	((char *)-1)
#endif

#define MM_ALLOC_MINSIZE         8192

/*
 * Define a union with types which are likely to have the longest
 * *relevant* CPU-specific memory word alignment restrictions...
 */
union mem_word {
	char		(*mw_fp)(int);
	char		*mw_cp;
	long		mw_l;
	double		mw_d;
	intptr_t	mw_it;
};

typedef	union mem_word	mem_word;

#define	SIZEOF_mem_word	(sizeof(mem_word))

/*
 * Define the structure used for memory chunks
 */
union mem_chunk_mc_u {
	struct mem_chunk	*mc_next;	/* really used when it's free */
	mem_word		mc_base;/* virtually used when it's allocated */
};

struct mem_chunk {
	size_t			mc_size;	/* physical size */
	size_t			mc_usize;	/* user known size */
	struct mem_pool		*mc_pool;
	union mem_chunk_mc_u	mc_u;
};

typedef struct mem_chunk mem_chunk;

#define SIZEOF_mem_chunk (sizeof(mem_chunk)-sizeof(union mem_chunk_mc_u))

/*
 * Define the structure describing a memory pool
 */
struct mem_pool {
	struct mem_pool	*mp_prev;
	struct mem_pool	*mp_next;
	size_t		mp_size;
	size_t		mp_offset;
	int		mp_users;
	mem_chunk	mp_freechunks;
	mem_word	mp_base;
};

typedef struct mem_pool mem_pool;

#define SIZEOF_mem_pool (sizeof(mem_pool)-SIZEOF_mem_word)

/*
 * Define the structure describing a shared memory core area
 * (the actual contents depends on the shared memory and
 * semaphore/mutex type and is stripped down to a minimum
 * required)
 */
struct mem_core {
	size_t		mo_size;
	size_t		mo_usize;
	mem_word	mo_base;
};

typedef struct mem_core mem_core;

#define SIZEOF_mem_core (sizeof(mem_core)-SIZEOF_mem_word)

typedef mem_pool MM;

static MM	*mm_global = NULL;
#ifndef	MAP_ANON
static int	zerofd = -1;
#endif

extern int	error(char *, ...);

/*
 * Determine memory page size of OS
 */
static size_t 
mm_core_pagesize(void)
{
	static int pagesize = 0;

	if (pagesize == 0)
#ifdef	_SC_PAGESIZE
		pagesize = sysconf(_SC_PAGESIZE);
#else
		pagesize = 8192;
#endif
	return pagesize;
}

/*
 * Align a size to the next page or word boundary
 */
static size_t
mm_core_align2page(size_t size)
{
	int psize = mm_core_pagesize();
	return ((size) % (psize) > 0 ?
		((size) / (psize) + 1) * (psize) : (size));
}

static size_t
mm_core_align2word(size_t size)
{
	return ((1 + ((size - 1) / SIZEOF_mem_word)) * SIZEOF_mem_word);
}

/*
 * Create a shared memory area
 */
static char
*mm_core_create(size_t usersize)
{
	mem_core *mo;
	char *area = ((char *)-1);
	size_t size;

	if (usersize <= 0) {
		errno = EINVAL;
		return NULL;
	}
	size = mm_core_align2page(usersize + SIZEOF_mem_core);
#ifdef	MAP_ANON
	if ((area = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_PRIVATE, -1, 0)) == (char *)MAP_FAILED)
		error(catgets(catd, 1, 243,
		"failed to memory map anonymous area"));
#else	/* !MAP_ANON */
	if (zerofd == -1 && ((zerofd = open("/dev/zero", O_RDWR)) == -1
#ifdef	F_SETFD
#ifndef	FD_CLOEXEC
#define	FD_CLOEXEC	1
#endif	/* !FD_CLOEXEC */
			|| fcntl(zerofd, F_SETFD, FD_CLOEXEC) == -1
#endif	/* F_SETFD */
			    ))
		error(catgets(catd, 1, 244,
			"failed to open /dev/zero"));
	if ((area = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE, zerofd, (off_t)0)) == (char *)MAP_FAILED)
		error(catgets(catd, 1, 245,
			"failed to memory map /dev/zero"));
#endif	/* !MAP_ANON */
	/*
	 * Configure the memory core parameters
	 */
	mo = (mem_core *)area;
	mo->mo_size = size;
	mo->mo_usize = usersize;
	/*
	 * Return successfully established core
	 */
	return ((char *)&(mo->mo_base.mw_cp));
}

/*
 * Create a memory pool
 */
static MM *
mm_create(MM *mmPrev, size_t usize)
{
	MM *mm = NULL;
	char *core;
	size_t size;

	/*
	 * defaults 
	 */
	if (usize < MM_ALLOC_MINSIZE)
		usize = MM_ALLOC_MINSIZE;
	/*
	 * determine size 
	 */
	size = usize + SIZEOF_mem_pool;
	/*
	 * get a shared memory area 
	 */
	if ((core = mm_core_create(size)) == NULL)
		return NULL;
	/*
	 * fill in the memory pool structure 
	 */
	mm = (MM *)core;
	mm->mp_prev = mmPrev;
	mm->mp_next = NULL;
	mm->mp_size = size;
	mm->mp_offset = SIZEOF_mem_pool;
	mm->mp_users = 0;
	/*
	 * first element of list of free chunks counts existing chunks 
	 */
	mm->mp_freechunks.mc_size = 0;	/* has to be 0 forever */
	mm->mp_freechunks.mc_usize = 0;	/* counts chunks */
	mm->mp_freechunks.mc_u.mc_next = NULL;
	return mm;
}

/*
 * Insert a chunk to the list of free chunks. Algorithm used is:
 * Insert in sorted manner to the list and merge with previous
 * and/or next chunk when possible to form larger chunks out of
 * smaller ones.
 */
static void
mm_insert_chunk(MM *mm, mem_chunk *mcInsert)
{
	mem_chunk *mc;
	mem_chunk *mcPrev;
	mem_chunk *mcNext;

	mc = &(mm->mp_freechunks);
	while (mc->mc_u.mc_next != NULL &&
			(char *)(mc->mc_u.mc_next) < (char *)mcInsert)
		mc = mc->mc_u.mc_next;
	mcPrev = mc;
	mcNext = mc->mc_u.mc_next;
	if (mcPrev == mcInsert || mcNext == mcInsert)
		error(catgets(catd, 1, 246,
		"chunk of memory already in free list"));
	if ((char *)mcPrev + (mcPrev->mc_size) == (char *)mcInsert &&
			(mcNext != NULL &&
			 (char *)mcInsert + (mcInsert->mc_size) ==
			 (char *)mcNext)) {
		/*
		 * merge with previous and next chunk 
		 */
		mcPrev->mc_size += mcInsert->mc_size + mcNext->mc_size;
		mcPrev->mc_u.mc_next = mcNext->mc_u.mc_next;
		mm->mp_freechunks.mc_usize -= 1;
	} else if ((char *)mcPrev + (mcPrev->mc_size) ==
		   (char *)mcInsert) {
		/*
		 * merge with previous chunk 
		 */
		mcPrev->mc_size += mcInsert->mc_size;
	} else if (mcNext != NULL &&
			(char *)mcInsert + (mcInsert->mc_size) ==
			(char *)mcNext) {
		/*
		 * merge with next chunk 
		 */
		mcInsert->mc_size += mcNext->mc_size;
		mcInsert->mc_u.mc_next = mcNext->mc_u.mc_next;
		mcPrev->mc_u.mc_next = mcInsert;
	} else {
		/*
		 * no merging possible, so insert as new chunk 
		 */
		mcInsert->mc_u.mc_next = mcNext;
		mcPrev->mc_u.mc_next = mcInsert;
		mm->mp_freechunks.mc_usize += 1;
	}
}

/*
 * Retrieve a chunk from the list of free chunks.  Algorithm used
 * is: Search for minimal-sized chunk which is larger or equal
 * than the request size. But when the retrieved chunk is still a
 * lot larger than the requested size, split out the requested
 * size to not waste memory.
 */
static mem_chunk *
mm_retrieve_chunk(MM *mm, size_t size)
{
	mem_chunk *mc;
	mem_chunk **pmcMin;
	mem_chunk *mcRes;
	size_t sMin;
	size_t s;

	if (size == 0)
		return NULL;
	if (mm->mp_freechunks.mc_usize == 0)
		return NULL;
	/*
	 * find best-fitting chunk 
	 */
	pmcMin = NULL;
	sMin = mm->mp_size;	/* initialize with maximum possible */
	mc = &(mm->mp_freechunks);
	while (mc->mc_u.mc_next != NULL) {
		s = mc->mc_u.mc_next->mc_size;
		if (s >= size && s < sMin) {
			pmcMin = &(mc->mc_u.mc_next);
			sMin = s;
			if (s == size)
				break;
		}
		mc = mc->mc_u.mc_next;
	}
	/*
	 * create result chunk 
	 */
	if (pmcMin == NULL)
		mcRes = NULL;
	else {
		mcRes = *pmcMin;
		if (mcRes->mc_size >= (size + min_of(2 * size, 128))) {
			/*
			 * split out in part 
			 */
			s = mcRes->mc_size - size;
			mcRes->mc_size = size;
			/*
			 * add back remaining chunk part as new chunk 
			 */
			mc = (mem_chunk *)((char *)mcRes + size);
			mc->mc_size = s;
			mc->mc_u.mc_next = mcRes->mc_u.mc_next;
			*pmcMin = mc;
		} else {
			/*
			 * split out as a whole 
			 */
			*pmcMin = mcRes->mc_u.mc_next;
			mm->mp_freechunks.mc_usize--;
		}
	}
	return mcRes;
}

/*
 * Allocate a chunk of memory
 */
char *
malloc(size_t usize)
{
	MM *mm;
	mem_chunk *mc;
	size_t size;

	size = mm_core_align2word(SIZEOF_mem_chunk + usize);
	if (mm_global == NULL && (mm_global = mm_create(NULL, size)) == NULL)
		return NULL;
	mm = mm_global;
nextpool:
	if ((mc = mm_retrieve_chunk(mm, size)) != NULL) {
		mc->mc_usize = usize;
		mc->mc_pool = mm;
		mm->mp_users++;
		return (char *)&(mc->mc_u.mc_base.mw_cp);
	}
	if ((mm->mp_size - mm->mp_offset) < size) {
		if (mm->mp_next != NULL ||
				(mm->mp_next = mm_create(mm, size)) != NULL) {
			mm = mm->mp_next;
			goto nextpool;
		}
		error(catgets(catd, 1, 247, "out of memory"));
		errno = ENOMEM;
		return NULL;
	}
	mc = (mem_chunk *)((char *)mm + mm->mp_offset);
	mc->mc_size = size;
	mc->mc_usize = usize;
	mc->mc_pool = mm;
	mm->mp_offset += size;
	mm->mp_users++;
	return (char *)&(mc->mc_u.mc_base.mw_cp);
}

/*
 * Free a chunk of memory
 */
void
free(char *ptr)
{
	MM *mm;
	mem_chunk *mc;

	if (mm_global == NULL || ptr == NULL)
		return;
	mc = (mem_chunk *)((char *)ptr - SIZEOF_mem_chunk);
	mm = mc->mc_pool;
	mm_insert_chunk(mm, mc);
	if (--mm->mp_users == 0) {
		mem_core *mo = (mem_core *)((char *)mm - SIZEOF_mem_core);
		if (mm == mm_global)
			mm_global = mm->mp_next;
		if (mm->mp_prev != NULL)
			mm->mp_prev->mp_next = mm->mp_next;
		if (mm->mp_next != NULL)
			mm->mp_next->mp_prev = mm->mp_prev;
		munmap((char *)mo, mo->mo_size);
	}
}

/*
 * Reallocate a chunk of memory
 */
char *
realloc(char *ptr, size_t usize)
{
	size_t size;
	mem_chunk *mc;
	char *vp;

	if (ptr == NULL)
		return malloc(usize);	/* POSIX.1 semantics */
	mc = (mem_chunk *)((char *)ptr - SIZEOF_mem_chunk);
	if (usize <= mc->mc_usize) {
		mc->mc_usize = usize;
		return ptr;
	}
	size = mm_core_align2word(SIZEOF_mem_chunk + usize);
	if (size <= mc->mc_size) {
		mc->mc_usize = usize;
		return ptr;
	}
	if ((vp = malloc(usize)) == NULL)
		return NULL;
	memcpy(vp, ptr, usize);
	free(ptr);
	return vp;
}

/*
 * Allocate and initialize a chunk of memory
 */
char *
calloc(size_t number, size_t usize)
{
	char *vp;

	if ((vp = malloc(number * usize)) == NULL)
		return NULL;
	memset(vp, 0, number * usize);
	return vp;
}

/*ARGSUSED*/
void
cfree(char *p, size_t num, size_t size)
{
	free(p);
}

#ifdef	notdef
/*ARGSUSED*/
char *
memalign(size_t alignment, size_t size)
{
	return NULL;
}

/*ARGSUSED*/
char *
valloc(size_t size)
{
	return NULL;
}

char *
mallinfo(void)
{
	return NULL;
}

int 
mallopt(void)
{
	return -1;
}
#endif	/* notdef */

/*ARGSUSED*/
char *
poolsbrk(intptr_t val)
{
	return NULL;
}
