/*
 * fsetflags.c		- Set a file flags on an ext2 file system
 *
 * Copyright (C) 1993, 1994  Remy Card <card@masi.ibp.fr>
 *                           Laboratoire MASI, Institut Blaise Pascal
 *                           Universite Pierre et Marie Curie (Paris VI)
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Library
 * General Public License, version 2.
 * %End-Header%
 */

/*
 * History:
 * 93/10/30	- Creation
 */

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#define EXT2_IOC_SETFLAGS		_IOW('f', 2, long)

/*
 * Deal with lame glibc's that define this function without actually
 * implementing it.  Can you say "attractive nuisance", boys and girls?
 * I knew you could!
 */
#ifdef __linux__
#undef HAVE_CHFLAGS
#endif

#ifdef O_LARGEFILE
#define OPEN_FLAGS (O_RDONLY|O_NONBLOCK|O_LARGEFILE)
#else
#define OPEN_FLAGS (O_RDONLY|O_NONBLOCK)
#endif

int fsetflags (const char * name, unsigned long flags)
{
	int fd, r, f, save_errno = 0;
	struct stat buf;

	if (!lstat(name, &buf) &&
	    !S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode)) {
		goto notsupp;
	}
	fd = open (name, OPEN_FLAGS);
	if (fd == -1)
		return -1;
	f = (int) flags;
	r = ioctl (fd, EXT2_IOC_SETFLAGS, &f);
	if (r == -1)
		save_errno = errno;
	close (fd);
	if (save_errno)
		errno = save_errno;
	return r;

notsupp:
	errno = EOPNOTSUPP;
	return -1;
}
