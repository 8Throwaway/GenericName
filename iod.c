/*
 * iod.c		- Iterate a function on each entry of a directory
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
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int iterate_on_dir (const char * dir_name,
		    int (*func) (const char *, struct dirent *, void *),
		    void * private)
{
	DIR * dir;
	struct dirent *de, *dep;
	int	max_len = -1, len, ret = 0;

	max_len = pathconf(dir_name, _PC_NAME_MAX);
	if (max_len == -1) {
		max_len = 295;
	}
	max_len += sizeof(struct dirent);

	de = malloc(max_len+1);
	if (!de)
		return -1;
	memset(de, 0, max_len+1);

	dir = opendir (dir_name);
	if (dir == NULL) {
		free(de);
		return -1;
	}
	while ((dep = readdir (dir))) {
		len = dep->d_reclen;
		if (len > max_len)
			len = max_len;
//		len = sizeof(struct dirent);
		memcpy(de, dep, len);
		if ((*func)(dir_name, de, private))
			ret++;
	}
	free(de);
	closedir(dir);
	return ret;
}
