/*
 * Small help functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "include/common.h"

int get_terminal_width()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

size_t sum_length (char *files[], size_t num)
{
	size_t sum = 0;
	for(int i = 0; i < num; ++i)
		sum += strlen(files[i]);

	return sum;
}

void current_directory(char *str, int size)
{
	char path[size];
	if( !getcwd(path, size))
		sys_error("getcwd error");
	strncpy(str, path, size);
}

int existing_directory(char *dir)
{
	DIR *d;
	if(!(d = opendir(dir)))
		return 0;
	closedir(d);
	return 1;
}

int st_mode_value(char *file)
{
	struct stat sb;

	if(lstat(file, &sb))          // not stat because currect S_ISLNK(m) usage
		sys_error("lstat error");

	if(S_ISLNK(sb.st_mode))
		return IS_LNK;
	else if(S_ISDIR(sb.st_mode))
		return IS_DIR;
	else if(sb.st_mode & S_IXUSR)
		return IS_EXE;
	else
		return IS_PLAIN;
}

void get_file_info(char *file, char perm[], int *value)
{
	struct stat sb;

	if(lstat(file, &sb))
		sys_error("lstat error");

	if(S_ISLNK(sb.st_mode))
	{
		*value = IS_LNK;
		perm[0] = 'l';
	}
	else if(S_ISDIR(sb.st_mode))
	{
		*value = IS_DIR;
		perm[0] = 'd';
	}
	else if(sb.st_mode & S_IXUSR)
	{
		*value = IS_EXE;
		perm[0] = '-';
	}
	else
	{
		*value = IS_PLAIN;
		perm[0] = '-';
	}

	if(sb.st_mode & S_IRUSR)
		perm[1] = 'r';
	else
		perm[1] = '-';
	if(sb.st_mode & S_IWUSR)
		perm[2] = 'w';
	else
		perm[2] = '-';
	if(sb.st_mode & S_IXUSR)
		perm[3] = 'x';
	else
		perm[3] = '-';

	if(sb.st_mode & S_IRGRP)
		perm[4] = 'r';
	else
		perm[4] = '-';
	if(sb.st_mode & S_IWGRP)
		perm[5] = 'w';
	else
		perm[5] = '-';
	if(sb.st_mode & S_IXGRP)
		perm[6] = 'x';
	else
		perm[6] = '-';

	if(sb.st_mode & S_IROTH)
		perm[7] = 'r';
	else
		perm[7] = '-';
	if(sb.st_mode & S_IWOTH)
		perm[8] = 'w';
	else
		perm[8] = '-';
	if(sb.st_mode & S_IXOTH)
		perm[9] = 'x';
	else
		perm[9] = '-';
}

void *xrealloc(void *ptr, size_t bytes)
{
	ptr = realloc(ptr, bytes);
	if(!ptr)
		sys_error("realloc error");
	return ptr;
}

void user_error(char *e, ...)
{
	va_list(args);
	va_start(args, e);
	vprintf(e, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

void sys_error(char *e)
{
	perror(e);
	exit(EXIT_FAILURE);
}
