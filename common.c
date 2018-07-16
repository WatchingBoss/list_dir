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

int find_middle(char *files[], size_t num)
{
	size_t middle = 0, sum = 0;
	for(int i = 0; i < num; ++i)
		sum += strlen(files[i]);

	middle = sum / num;

	return middle;
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

int executable_file(char *file)
{
	struct stat sb;
	if(!stat(file, &sb) && sb.st_mode & S_IXUSR)
		return 1;
	return 0;
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
