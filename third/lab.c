//Вариант 2

/*Написать программу синхронизации двух каталогов, например, Dir1 и Dir2. Пользователь задаёт имена Dir1 и Dir2.
В результате работы программы файлы, имеющиеся в Dir1, но отсутствующие в Dir2, должны скопироваться в Dir2 вместе с правами доступа.
Процедуры копирования должны запускаться в отдельном процессе для каждого копируемого файла. Каждый процесс выводит на экран свой pid,
полный путь к копируемому файлу и число скопированных байт. Число одновременно работающих процессов не должно превышать N (вводится пользователем).
Скопировать несколько файлов из каталога /etc в свой домашний каталог. Проверить работу программы для каталога /etc и домашнего каталога. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER 16*1024*1024

char BUF[BUFFER];
char *script_name = NULL;
int MAX_PROCESSES = 0;
int status = 0;
int process_count = 0;

int take_amount_from_string(char*);
void p_error(int, char*, char*, char*); 
void p_result(char*, int);
void copy(char*, char*, int*);
void directory(char*, char*);


void main(int argc, char *argv[])
{
	script_name = basename(argv[0]);

	if (argc != 4)
	{
		fprintf(stderr, "%s: %s\n", script_name, "Неверное кол-во аргументов."); 
		exit(1);
	}


    char *dir1 = realpath(argv[1], NULL);
	char *dir2 = realpath(argv[2], NULL);

	if (dir1 == NULL)
	{
		fprintf(stderr, "%s: %s - %s\n", script_name, "Недопустимое значение каталога.", argv[1]); 
    		exit(1);
	}
	if (dir2 == NULL)
	{
		fprintf(stderr, "%s: %s - %s\n", script_name, "Недопустимое значение каталога.", argv[2]); 
		exit(1);
	}

	MAX_PROCESSES = take_amount_from_string(argv[3]) - 1;
	if (MAX_PROCESSES == -1)
	{
		fprintf(stderr, "%s: %s\n", script_name, "Недопустимое значение процессов.");
    		exit(1);
	}

	directory(dir1, dir2);

	int res;
	errno = 0;
	while ((res = wait(NULL)) > 0)
    	{
    		if (errno != 0)
    		{
    			p_error(getpid(), script_name, strerror(errno), NULL);
        		errno = 0;
    		}
    	}
	exit(0);
}

void p_error(int pid, char *scr_name, char *msg_err, char *add_msg) 
{
    fprintf(stderr, "%d %s: %s %s\n", pid, scr_name, msg_err, (add_msg)? add_msg : NULL);
}

void p_result(char *dir, int bytes_count)
{
    printf("%d %s %d\n", getpid(), dir, bytes_count);
}


void copy(char *current_dir1,  char *current_dir2, int *bytes_count) 
{
    mode_t mode;
    struct stat file_information;
    if (access(current_dir1, R_OK) == -1) {
    	p_error(getpid(), script_name,   strerror(errno), current_dir1);
    	return;
    }
    if ((stat(current_dir1, &file_information) == -1)) {
    	p_error(getpid(), script_name, strerror(errno), current_dir1);
    	return;
    }

    mode = file_information.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO);

    int file_source = open(current_dir1,  O_RDONLY);
    int file_destination = open(current_dir2, O_WRONLY|O_CREAT|O_TRUNC, mode);

    if (file_source == -1) {
        p_error(getpid(), script_name, "Ошибка открытия файла.", current_dir1);
        return;
    }
    if (file_destination == -1) {
        p_error(getpid(), script_name,   "Ошибка открытия файла.", current_dir2);
        return;
    }

    *bytes_count = 0;
    
/*
	char *buff;
    if ((buff = (char *) malloc(BUFFER)) == NULL) {
    	p_error(getpid(), script_name,   "Выход за пределы памяти.", NULL);
    	return;
    }
*/

    ssize_t read_val, write_val;
    while (((read_val = read(file_source, BUF, BUFFER)) != 0) && (read_val != -1)) {

    	if ((write_val = write(file_destination, BUF, (size_t)read_val)) == -1) {
    	    p_error(getpid(), script_name,   "Ошибка записи в файл.", current_dir2);

    	    if (close(file_source) == -1)
    	    {
            	p_error(getpid(), script_name,   "Ошибка закрытия файла.", current_dir1);
            	return;
        	}
    	    if (close(file_destination) == -1)
    	    {
            	p_error(getpid(), script_name,   "Ошибка закрытия файла.", current_dir2);
            	return;
        	}
    	    return;
    	}
    	*bytes_count += write_val;
    }

    if (close(file_source) == -1) {
        p_error(getpid(), script_name,   "Ошибка закрытия файла.", current_dir1);
        return;
    }
    if (close(file_destination) == -1) {
        p_error(getpid(), script_name,   "Ошибка закрытия файла.", current_dir2);
        return;
    }
}


void directory(char *dir1, char *dir2)
{
	DIR *copied_dir = opendir(dir1);
	if (!copied_dir)
	{
    		p_error(getpid(), script_name, strerror(errno), dir1);
    		return;
	}

    char *current_dir1 = alloca(strlen(dir1) + NAME_MAX + 3);
    current_dir1[0] = 0;
    strcat(strcat(current_dir1, dir1), "/");
    size_t current_len1 = strlen(current_dir1);

    char *current_dir2 = alloca(strlen(dir2) + NAME_MAX + 3);
    current_dir2[0] = 0;
    strcat(strcat(current_dir2, dir2), "/");
    size_t current_len2 = strlen(current_dir2);

    struct dirent *selected_dir;
    struct stat buf1, buf2;
    errno = 0;

    while ( (selected_dir = readdir(copied_dir) ) != NULL)
    {
        current_dir1[current_len1] = 0;
        strcat(current_dir1, selected_dir->d_name);
	    current_dir2[current_len2] = 0;
        strcat(current_dir2, selected_dir->d_name);

        if ((lstat(current_dir1, &buf1) == -1))
        {
    	    p_error(getpid(), script_name, strerror(errno), current_dir1);
    	    continue;
	    }
        // если директория
        if ( S_ISDIR(buf1.st_mode))
        {

            if ( (strcmp(selected_dir->d_name, ".") != 0) && (strcmp(selected_dir->d_name, "..") != 0) )
            {
                if (stat(current_dir2, &buf2) != 0)
                {
                    mkdir(current_dir2, buf1.st_mode);
    		    }
    		    directory(current_dir1, current_dir2);
            }
        }
        else //если файл
            if ( S_ISREG(buf1.st_mode))
            {
                if (process_count > MAX_PROCESSES - 1)
                {
                    if (wait(NULL) != -1)
                    {
                        --process_count;
                    }
                }

        		pid_t pid = fork();

                if (pid == 0)
                {
                	int bytes_count = 0;
                	copy(current_dir1, current_dir2, &bytes_count);
            		if (bytes_count != 0)
            		{
                		p_result(current_dir1, bytes_count);
            		}
            		exit(EXIT_SUCCESS);
                }
                if (pid < 0)
                {
                	p_error(getpid(), script_name, "Ошибка с образованием процесса.", NULL);
                }
                process_count++;
            }
            errno = 0;
    }
    if (errno != 0)
    {
    	fprintf(stderr, "%s: %s\n", script_name, strerror(errno));
    }

    if (closedir(copied_dir) == -1) {
        p_error(getpid(), script_name, strerror(errno), dir1);
    }
}

int take_amount_from_string(char* str)
{
	int result = 0;
	int length = strlen(str);
	for (int i = 0; i < length; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			return -1;
		}
		if (i == length - 1)
		{
			result += str[i] - '0';
		} else {
			result += str[i] - '0';
			result *= 10;
		}
	}
	return result;
}
