

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
#define FIBMAP     _IO(0x00,1)  /* bmap access */
#define FIGETBSZ   _IO(0x00,2)  /* get the block size used for bmap */
#define EXT4_EXTENTS_FL         0x00080000 /* Inode uses extents */
#define EXT3_IOC_GETFLAGS       _IOR('f', 1, long)

static void frag_report(const char *filename)
{
    struct statfs fsinfo;
#ifdef HAVE_FSTAT64
    struct stat64 fileinfo;
#else
    struct stat fileinfo;
#endif
    int bs;
    int fd;
    unsigned long numblocks;

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
    {

        printf("Filesystem type is: %lx\n",
               (unsigned long) fsinfo.f_type);
    }

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

    numblocks = ((size_t)fileinfo.st_size + ((unsigned)bs-1)) / (unsigned)bs;

    printf("File size of %s is %lld (%ld blocks of %d bytes)\n", filename,
        (long long) fileinfo.st_size, numblocks,bs);


    close(fd);
}

void part2(const char *filename)
{
    int fd = open (filename, O_RDONLY | O_NONBLOCK); //nonblock is advized by ioctl man
    if (fd == -1) {
        perror (filename);
        return ;
    }
    struct stat st = {0};
    if (fstat (fd, &st)) {
        perror (filename);
        return ;
    }
    struct statfs st_fs = {0};
    if (fstatfs (fd, &st_fs)) {
        perror (NULL);
     return ;
    }

    int blksize = 0;
    if (ioctl (fd, FIGETBSZ, &blksize)< 0) {
            perror ("ioctl failed to get fiemap");
            close (fd);
            return ;
    }
    char buf[2048] = {0};
    struct fiemap *fiemap = (struct fiemap *) buf; // why that unobvious? becaue fm_extemts are not fiemap_extent * but fiemap_extent [0]! WHY? Bacause!
    fiemap->fm_extent_count = (sizeof (buf) - sizeof (*fiemap)) / sizeof (struct fiemap_extent);
    fiemap->fm_length = ~ (uint64_t) 0;
    fiemap->fm_start = 0;
    char file_not_empty = 0, header_printed = 0;

    while (1) {
        if (ioctl (fd, FS_IOC_FIEMAP, fiemap) < 0) {
            perror ("ioctl failed to get fiemap");
            close (fd);
            return ;
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
                fiemap->fm_extents[i].fe_logical / (unsigned long long)blksize,    \
                fiemap->fm_extents[i].fe_physical / (unsigned long long)blksize,   \
                fiemap->fm_extents[i].fe_length / (unsigned long long)blksize);
        }
        fiemap->fm_start = fiemap->fm_extents[i-1].fe_logical + fiemap->fm_extents[i-1].fe_length;
    }
    close (fd);

}

int main(int argc, char**argv)
{
    if (argc < 2) {
        printf ("Usage: %s <file>\n", argv[0]);
        return 2;
    }
    for (int i = 1; i < argc; i++) {
        frag_report(argv[i]);
        part2(argv[i]);
    }

    return 0;
}
#endif