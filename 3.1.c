#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

int good_close(int fd)
{
    if (close(fd) < 0)
    {
        perror("Failure during close");
        return 9;
    }

    return 0;
}

ssize_t write_all(int fd, const void *buf, size_t count)
{
    size_t bytes_written = 0;
    const uint8_t *buf_adder = buf;
    while (bytes_written < (size_t)count)
    {
        ssize_t res = write(fd, buf_adder + bytes_written, count - bytes_written);
        if (res < 0)
            return res;
        bytes_written += res;
    }
    return (ssize_t)bytes_written;
}

int my_copy(int fd_r, int fd_w)
{
    char *str_tmp;

    str_tmp = (char *)malloc(BUF_SIZE);
    if (str_tmp == NULL)
    {
        perror("Faild to allocate the memory");
        return 6;
    }

    int check;

    while ((check = read(fd_r, str_tmp, BUF_SIZE)) != 0)
    {
        if (check < 0)
        {
            perror("Failed to read the file");
            return 7;
        }

        if (write_all(fd_w, str_tmp, strlen(str_tmp)) < 0)
        {
            perror("Failed to write");
            return 8;
        }
    }

    free(str_tmp);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s path text", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], argv[2]) == 0)
    {
        perror("You can't copy the file to itself");
        return 2;
    }

    int fd_r = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);

    if (fd_r < 0)
    {
        perror("Failed to open file for reading");
        return 3;
    }

    int fd_w = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd_w < 0)
    {
        perror("Failed to open file for writing");
        return 4;
    }

    int result = my_copy(fd_r, fd_w);

    if (good_close(fd_r) + good_close(fd_w) > 0)
        return 5;

    if (result > 0)
        return result;

    return 0;
}