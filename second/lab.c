// Вариант 9

/*Подсчитать для заданного каталога (первый аргумент командной строки) и всех его подкаталогов (по-отдельности) 
суммарный размер занимаемого файлами на диске пространства в байтах и суммарный размер файлов. 
Вычислить коэффициент использования дискового пространства в %. Для получения размера занимаемого файлами 
на диске пространства использовать команду stat. */

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned long* count_space(char*, unsigned long*);

void main(int argc, char* argv[]){
	if (argc != 2) {
		fprintf(stderr, "%s", "error: invalid number of args\n\r");
		return;
	}
	char *string_dir = argv[1];

	unsigned long result[2];
	count_space(string_dir, result);
}

unsigned long* count_space(char* path, unsigned long* array){
	DIR *directory;
        directory = opendir(path);
        if (directory == NULL) {
                char err[256] = {0};
                sprintf(err, "%s %s", path, "opendir()");
                perror(err);
                array[0] = 0;
		array[1] = 0;
		return array;
       	}

        struct dirent *read_dir;
	struct stat stats;
	unsigned long real_space = 0, used_space = 0;
	errno = 0;
	read_dir = readdir(directory);

	while(read_dir != NULL) {
		char buf[512] = {0};
		char cwd[256] = {0};
		unsigned long space[2] = {0};
		sprintf(buf, "%s/%s", path, read_dir->d_name);
		int res = stat(buf, &stats);

		if (res != 0){
			char info[256] = {0};
                	sprintf(info, "%s %s", path, "stat()");
                	perror(info);
			read_dir = readdir(directory);
			errno = 0;
			continue;
		}

		if ((stats.st_mode & S_IFMT) == S_IFDIR){
			if (!((strcmp(read_dir->d_name, ".") == 0) || (strcmp(read_dir->d_name, "..") == 0))){
	                        real_space += stats.st_size;
        	                used_space += (stats.st_blocks * stats.st_blksize);
				count_space(buf, space);
				real_space += space[0];
				used_space += space[1];
			}
		} else {
			real_space += stats.st_size;
                	used_space += (stats.st_blocks * stats.st_blksize);
		}
		read_dir = readdir(directory);
	}

	if (errno != 0) {
		char err_info[256] = {0};
                sprintf(err_info, "%s %s", path, "readdir()");
                perror(err_info);
	}

	int close_status = closedir(directory);
	if (close_status != 0) {
		char buf[256] = {0};
		sprintf(buf, "%s %s", path, "closedir()");
                perror(buf);
        }

	char canon_path[256];
	realpath(path, canon_path);
        fprintf(stdout, "%s %.2f%% %lu %lu\n", canon_path, ((float)real_space / used_space) * 100, real_space, used_space);
        array[0] = real_space;
	array[1] = used_space;
	return array;
}
