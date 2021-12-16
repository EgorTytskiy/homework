#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include <unistd.h> 


int main(int argc, char * argv[]) 
{
	//Выход с ошибкой, если неправильный ввод
	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s path text", argv[0]); 
		return 1; 
	}

	//Создание переменной fd
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); //Если файл не открылся для записи, fd = -1; если файла нет, то создаём, если в файле есть содержимое, стираем его



	//Выход с ошибкой, если файл не окрылся для чтения
	if (fd < 0) 
	{
		perror("File cannot be opened for writing"); //вывод в поток ошибок
		return 2;
	}

	if (dprintf(fd,"%s", argv[2]) < 0)
	{
		perror("Failure during writing"); 
		close(fd); 
		return 3;
	}

	
	//Выход с ошибкой, если файл не закрылся
	if (close(fd) < 0) 
	{
		perror("Failure during close"); 
		return 4; 
	}

	return 0; 
}