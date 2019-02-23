/*
 * pf.c			- Print file attributes on an ext2 file system
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

#include <stdio.h>
#define PFOPT_LONG 1
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


struct flags_name {
	unsigned long	flag;
	const char	*short_name;
	const char	*long_name;
};

static struct flags_name flags_array[] = {
	{ EXT2_SECRM_FL, "s", "Secure_Deletion" },
	{ EXT2_UNRM_FL, "u" , "Undelete" },
	{ EXT2_SYNC_FL, "S", "Synchronous_Updates" },
	{ EXT2_DIRSYNC_FL, "D", "Synchronous_Directory_Updates" },
	{ EXT2_IMMUTABLE_FL, "i", "Immutable" },
	{ EXT2_APPEND_FL, "a", "Append_Only" },
	{ EXT2_NODUMP_FL, "d", "No_Dump" },
	{ EXT2_NOATIME_FL, "A", "No_Atime" },
	{ EXT2_COMPR_FL, "c", "Compression_Requested" },
	{ EXT4_ENCRYPT_FL, "E", "Encrypted" },
	{ EXT3_JOURNAL_DATA_FL, "j", "Journaled_Data" },
	{ EXT2_INDEX_FL, "I", "Indexed_directory" },
	{ EXT2_NOTAIL_FL, "t", "No_Tailmerging" },
	{ EXT2_TOPDIR_FL, "T", "Top_of_Directory_Hierarchies" },
	{ EXT4_EXTENTS_FL, "e", "Extents" },
	{ FS_NOCOW_FL, "C", "No_COW" },
	{ EXT4_CASEFOLD_FL, "F", "Casefold" },
	{ EXT4_INLINE_DATA_FL, "N", "Inline_Data" },
	{ EXT4_PROJINHERIT_FL, "P", "Project_Hierarchy" },
	{ EXT4_VERITY_FL, "V", "Verity" },
	{ 0, NULL, NULL }
};

void print_flags (FILE * f, unsigned long flags, unsigned options)
{
	int long_opt = (options & PFOPT_LONG);
	struct flags_name *fp;
	int	first = 1;

	for (fp = flags_array; fp->flag != 0; fp++) {
		if (flags & fp->flag) {
			if (long_opt) {
				if (first)
					first = 0;
				else
					fputs(", ", f);
				fputs(fp->long_name, f);
			} else
				fputs(fp->short_name, f);
		} else {
			if (!long_opt)
				fputs("-", f);
		}
	}
	if (long_opt && first)
		fputs("---", f);
}

