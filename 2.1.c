#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include <unistd.h> 

ssize_t write_all(int fd, const void *buf, size_t count) 
{
	size_t bytes_written = 0; 
	const uint8_t *buf_adder = buf; 
	

	while (bytes_written < (size_t) count) 
	{
		ssize_t res = write(fd, buf_adder + bytes_written, count - bytes_written); 
		if(res < 0) 
			return res; 
		bytes_written += res; 
			
	}
	return (ssize_t)bytes_written; 
}

int main(int argc, char * argv[]) 
{

	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s path text", argv[0]); 
		return 1; 
	}

	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); //Если файл не открылся для записи, fd = -1; если файла нет, то создаём, если в файле есть содержимое, стираем его


	if (fd < 0) 
	{
		perror("File cannot be opened for writing"); //вывод в поток ошибок
		return 2;
	}

	if (write_all(fd, argv[2], strlen(argv[2])) < 0)
	{
		perror("Failure during writing"); 
		close(fd); 
		return 3;
	}

	if (close(fd) < 0) 
	{
		perror("Failure during close"); 
		return 4; 
	}

	return 0; 
}