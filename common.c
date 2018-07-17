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
		sys_error("stat error");

	if(S_ISLNK(sb.st_mode))
		return IS_LNK;
	else if(S_ISDIR(sb.st_mode))
		return IS_DIR;
	else if(sb.st_mode & S_IXUSR)
		return IS_EXE;
	else
		return IS_PLAIN;
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
