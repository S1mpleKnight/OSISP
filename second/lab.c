// Вариант 9

/*Подсчитать для заданного каталога (первый аргумент командной строки) и всех его подкаталогов (по-отдельности) 
суммарный размер занимаемого файлами на диске пространства в байтах и суммарный размер файлов. 
Вычислить коэффициент использования дискового пространства в %. Для получения размера занимаемого файлами 
на диске пространства использовать команду stat. */

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#define BASE_IDENTS_BUFFER 1024
#define BLOCK_SIZE 512
#define false 0
#define true 1
typedef int bool;
long calculateSpace(char *);
void findDiskUsage();
bool checkStat(struct stat *, char *path);
char *progName;
ino_t *identificators;
long nIdents;
long diskSpace, filesSpace;
dev_t curDev;

int main(int nArgs, char **args)
{

	progName = basename(args[0]);

	if (nArgs != 2)
	{
		fprintf(stderr, "%s: Invalid number of arguments.\n", progName);
		return 1;
	}

	identificators = (ino_t *)calloc(BASE_IDENTS_BUFFER, sizeof(ino_t));
	nIdents = 0;

	char *parentDir = realpath(args[1], NULL);
	struct stat stat;
	if (lstat(parentDir, &stat) != 0)
	{
		fprintf(stderr, "%s: Invalid directory: %s.\n", progName, parentDir);
		return 1;
	}
	else
	{ //for device check
		curDev = stat.st_dev;
	}

	filesSpace = 0;
	diskSpace = 0;
	if ((diskSpace = calculateSpace(parentDir)) == -1)  //recursion point
	{
		fprintf(stderr, "%s: Can not find diretory space: %s.\n", progName, parentDir);
		//return 1;
	}

	if (diskSpace != 0)
	{
		printf("%ld %ld %f\n", filesSpace, diskSpace, ((double)filesSpace * 100) / diskSpace);
	}
	else
	{
		printf("%ld/0\n", filesSpace);
	}
	free(parentDir);
	free(identificators);
	return 0;
}

void findDiskUsage(const char *dirName)
{
	DIR *dir;
	long thisDiskSpace = 0;
	long thisFilesSpace = 0;
	if ((dir = opendir(dirName)) == NULL)
	{
		fprintf(stderr, "%s: %s: %s\n", progName, dirName, strerror(errno));
		return;
	}

	struct dirent *ent;

	for (;;)
	{
		errno = 0;
		ent = readdir(dir);

		if (ent == NULL)
		{
			if (errno == 0)
			{
				break;
			}
			else if (errno == EBADF)
			{
				fprintf(stderr, "%s: %s: %s\n", progName, dirName, strerror(errno));
			}
		}
		else
		{
			if (strncmp(ent->d_name, ".", PATH_MAX) == 0 || strncmp(ent->d_name, "..", PATH_MAX) == 0)
			{
				continue;
			}
			char path[PATH_MAX + 1];
			if (strncmp(dirName, "/", PATH_MAX) == 0)
			{
				snprintf(path, sizeof(path), "%s%s", dirName, ent->d_name);
			}
			else
			{
				snprintf(path, sizeof(path), "%s/%s", dirName, ent->d_name);
			}
			long tmp;
			if ((tmp = calculateSpace(path)) == -1)
			{
				continue;
			}
			else
			{
				struct stat stat;
				lstat(path, &stat);
				if (!S_ISDIR(stat.st_mode))
				{
					thisDiskSpace += tmp;
					thisFilesSpace += stat.st_size;
				}
				else
				{
					thisDiskSpace += BLOCK_SIZE * stat.st_blocks;
					thisFilesSpace += stat.st_size;
				}
			}
		}
	}


	fprintf(stdout,"%s: %ld %ld \n", dirName, (thisFilesSpace), (thisDiskSpace));

	if (closedir(dir) == -1)
	{
		fprintf(stderr, "%s: %s: %s\n", progName, dirName, strerror(errno));
	}
}

long calculateSpace(char *path)
{
	struct stat stat;
	long tmDiskUsage = diskSpace;
	if (lstat(path, &stat) != 0)
	{
		return -1;
	}

	if (S_ISLNK(stat.st_mode))
	{
		filesSpace += stat.st_size;
		diskSpace += BLOCK_SIZE * stat.st_blocks;
		return stat.st_size;
	}

	if (!checkStat(&stat, path))
	{
		return -1;
	}

	if (S_ISDIR(stat.st_mode))
	{
		filesSpace += stat.st_size;
		diskSpace += BLOCK_SIZE * stat.st_blocks;
		findDiskUsage(path);					  //Dir processing
		return diskSpace - tmDiskUsage;
	}

	if (S_ISREG(stat.st_mode))
	{
		filesSpace += stat.st_size;
		diskSpace += BLOCK_SIZE * stat.st_blocks; 
		return BLOCK_SIZE * stat.st_blocks; 
	}
	return -1;
}

bool checkStat(struct stat *stat, char *path)
{
	if (stat->st_dev != curDev)
	{
		return false;
	}

	if (stat->st_nlink > 1)
	{
		ino_t curIdent = stat->st_ino;
		for (int i = 0; i < nIdents; i++)
		{
			if (identificators[i] == curIdent)
			{
				return false;
			}
			//printf("%d\n",i);
		}
		if (nIdents % BASE_IDENTS_BUFFER > BASE_IDENTS_BUFFER - 2)
		{
			identificators = (ino_t *)realloc(identificators, (nIdents + BASE_IDENTS_BUFFER + 1) * sizeof(ino_t)); //REALLOC!!
		}
		identificators[nIdents++] = curIdent;
		//printf("________________________\n%ld\n",nIdents);
	}
	return true;
}
