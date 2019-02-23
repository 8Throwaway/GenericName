/*
 * fgetflags.c		- Get a file flags on an ext2 file system
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
#define O_NONBLOCK  0
#define O_RDONLY    _O_RDONLY
#define O_RDWR      _O_RDWR
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

#define EXT2_IOC_GETFLAGS		_IOR('f', 1, long)

#ifdef O_LARGEFILE
#define OPEN_FLAGS (O_RDONLY|O_NONBLOCK|O_LARGEFILE)
#else
#define OPEN_FLAGS (O_RDONLY|O_NONBLOCK)
#endif

int fgetflags (const char * name, unsigned long * flags)
{
	struct stat buf;

	int fd, r, f, save_errno = 0;

	if (!lstat(name, &buf) &&
	    !S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode)) {
		goto notsupp;
	}
	fd = open (name, OPEN_FLAGS);
	if (fd == -1)
		return -1;
	r = ioctl (fd, EXT2_IOC_GETFLAGS, &f);
	if (r == -1)
		save_errno = errno;
	*flags = f;
	close (fd);
	if (save_errno)
		errno = save_errno;
	return r;
notsupp:
	errno = EOPNOTSUPP;
	return -1;
}
