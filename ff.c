/*
 * filefrag.c -- report if a particular file is fragmented
 * 
 * Copyright 2003 by Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#ifndef __linux__
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    fputs("This program is only supported on Linux!\n", stderr);
    exit(EXIT_FAILURE);
}
#else
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern char *optarg;
extern int optind;
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <linux/fd.h>
//-----------------------------------------------------
#define _GNU_SOURCE

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sys/syscall.h>
#include <sys/xattr.h>

#include <linux/fs.h>

#include <sys/ioctl.h>
#include <linux/fiemap.h>
//---------------------------------------------------------------
int verbose = 0;

#define FIBMAP	   _IO(0x00,1)	/* bmap access */
#define FIGETBSZ   _IO(0x00,2)	/* get the block size used for bmap */

#define EXT4_EXTENTS_FL			0x00080000 /* Inode uses extents */
#define	EXT3_IOC_GETFLAGS		_IOR('f', 1, long)

static unsigned int div_ceil(unsigned int a, unsigned int b)
{
	if (!a)
		return 0;
	return ((a - 1) / b) + 1;
}

static unsigned long get_bmap(int fd, unsigned long block)
{
	int	ret;
	unsigned int b;

	b = block;
	ret = ioctl(fd, FIBMAP, &b); /* FIBMAP takes a pointer to an integer */
	if (ret < 0) {
		if (errno == EPERM) {
			//fprintf(stderr, "No permission to use FIBMAP ioctl; must have root privileges\n");
			exit(1);
		}
		perror("FIBMAP");
	}
	return b;
}

#define EXT2_DIRECT	12

static void frag_report(const char *filename)
{
	struct statfs	fsinfo;
#ifdef HAVE_FSTAT64
	struct stat64	fileinfo;
#else
	struct stat	fileinfo;
#endif
	int		bs;
	long		fd;
	unsigned long	block, last_block = 0, numblocks, i;
	long		bpib;	/* Blocks per indirect block */
	long		cylgroups;
	int		discont = 0, expected;
	int		is_ext2 = 0;
	unsigned int	flags;

	if (statfs(filename, &fsinfo) < 0) {
		perror("statfs");
		return;
	}
#ifdef HAVE_FSTAT64
	if (stat64(filename, &fileinfo) < 0) {
#else
	if (stat(filename, &fileinfo) < 0) {
#endif
		perror("stat");
		return;
	}
	if (!S_ISREG(fileinfo.st_mode)) {
		printf("%s: Not a regular file\n", filename);
		return;
	}
	if ((fsinfo.f_type == 0xef51) || (fsinfo.f_type == 0xef52) || 
	    (fsinfo.f_type == 0xef53))
		is_ext2++;
	if (verbose) {

		printf("Filesystem type is: %lx\n", 
		       (unsigned long) fsinfo.f_type);
	}
	cylgroups = div_ceil(fsinfo.f_blocks, fsinfo.f_bsize*8);

#ifdef HAVE_OPEN64
	fd = open64(filename, O_RDONLY);
#else
	fd = open(filename, O_RDONLY);
#endif
	if (fd < 0) {
		perror("open");
		return;
	}
	if (ioctl(fd, FIGETBSZ, &bs) < 0) { /* FIGETBSZ takes an int */
		perror("FIGETBSZ");
		close(fd);
		return;
	}
	if (ioctl(fd, EXT3_IOC_GETFLAGS, &flags) < 0)
		flags = 0;

     //printf("end of seredina part of 1\n");
	
	bpib = bs / 4;
	numblocks = (fileinfo.st_size + (bs-1)) / bs;
    if (verbose){

		printf("File size of %s is %lld (%ld blocks of %d bytes)\n", filename, 
		       (long long) fileinfo.st_size, numblocks,bs);

	}

	
	close(fd);
}

static void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s [-v] file ...\n", progname);
	exit(1);
}
void part1(int argc, char**argv)
{
  
	char **cpp;
	int c;

	while ((c = getopt(argc, argv, "v")) != EOF)
		switch (c) {
		case 'v':
			verbose++;
			break;
		default:
			usage(argv[0]);
			break;
		}
	if (optind == argc)
		usage(argv[0]);
		 //printf("end of part1\n");
	for (cpp=argv+optind; *cpp; cpp++) {
		if (verbose)
			printf("Checking %s\n", *cpp);
		frag_report(*cpp);

	}
	 //printf("end of part1\n");
}
//---------------------------------------------------
extern int errno;
extern char *optarg;
extern int optind, opterr, optopt;
 
typedef int fdesc;
void part2(int argc, char**argv)
{
int ret = 0;
    if (argc < 2) {
        printf ("Usage: %s <file>\n", argv[0]);
        //return -2;
    }
    char *filename = argv[argc - 1];
    fdesc fd = open (filename, O_RDONLY | O_NONBLOCK); //nonblock is advized by ioctl man
    if (fd == -1) {
        perror (filename);
        //return -2;
    }
    struct stat st = {0};
    if (fstat (fd, &st)) {
        perror (filename);
        //return -2;
    }
    struct statfs st_fs = {0};
    if (fstatfs (fd, &st_fs)) {
        perror (NULL);
        //return -2;
    }
    //printf ("fs type code is: %lx\n", (unsigned long) st_fs.f_type);
    blksize_t blksize = 0;
    
    //printf ("stat blksize is: %lu\n", (unsigned long) st.st_blksize);
    //printf ("statfs blksize is: %lu\n", (unsigned long) st_fs.f_bsize);
	int blt=!ioctl (fd, FIGETBSZ, &blksize);
    //if (!ioctl (fd, FIGETBSZ, &blksize))
           // printf ("ioctl blksize is: %lu\n", (unsigned long) blksize);
    //will use fiemap for extracting extents info
    char buf[2048] = {0};
    struct fiemap *fiemap = (struct fiemap *) buf; // why that unobvious? becaue fm_extemts are not fiemap_extent * but fiemap_extent [0]! WHY? Bacause!
    fiemap->fm_extent_count = (sizeof (buf) - sizeof (*fiemap)) / sizeof (struct fiemap_extent);
    fiemap->fm_length = ~ (uint64_t) 0;
    fiemap->fm_start = 0;
    char file_not_empty = 0, header_printed = 0;
    //printf ("file '%s':\n", filename);
    while (1) {
        ret = ioctl (fd, FS_IOC_FIEMAP, fiemap);
        if (ret < 0) {
            perror ("ioctl failed to get fiemap");
            close (fd);
            //return -1;
        }
        if (fiemap->fm_mapped_extents)
            file_not_empty = 1;
        else {
            if (!file_not_empty)
                printf ("is empty\n");
            break;
        }
        if (!header_printed) {
            printf ("%-4s%-24s%-24s%-24s\n", "N", "logical offset, blk", "physical offset, blk", "extent length, blk");
            header_printed = 1;
        }
        unsigned long i = 0;
        for (i = 0; i < fiemap->fm_mapped_extents; i++) {
            static unsigned long ext_num = 0;
            printf ("%-4lu%-24llu%-24llu%-24llu\n", ext_num++, \
                fiemap->fm_extents[i].fe_logical / blksize,    \
                fiemap->fm_extents[i].fe_physical / blksize,   \
                fiemap->fm_extents[i].fe_length / blksize);
        }
        fiemap->fm_start = fiemap->fm_extents[i-1].fe_logical + fiemap->fm_extents[i-1].fe_length;
    }
    close (fd);
    
}
//---------------------------------------------------
int main(int argc, char**argv)
{ 
    
 part1(argc,argv);
 //printf("end of part1\n");
 //-----------------------------------------------------------------
 part2(argc,argv);


 //-----------------------------------------------------------------
 return 0;

}
#endif