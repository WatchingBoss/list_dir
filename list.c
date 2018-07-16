/*
 * List directory linux terminal utility
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>

#include "include/list.h"

/* START COLORS */
#define AC_B_RED   "\x1b[91;1m"
#define AC_B_GREEN "\x1b[92;1m"
#define AC_B_BLUE  "\x1b[94;1m"
#define AC_WHITE   "\x1b[97;1m"
#define AC_RESET   "\x1b[0m"
/* END COLORS   */

#define S_PATH 256
#define OPTIONS "al::"

/* === START GLOBAL VARIABLE === */
bool print_begin_with_dot = false;
bool print_like_list = false;
/* === END GLOBAL VARIABLE   === */

void list_print_file(char *path, char *files[], size_t num)
{
//	char full_path[S_PATH];

//	memset(full_path, 0, S_PATH);
//	snprintf(full_path, S_PATH, "%s/%s", path, files[i]);
}

void simple_print_file(char *path, char *files[], size_t num)
{
	char full_path[S_PATH];
	size_t middle = find_middle(files, num);
	size_t win_width = get_terminal_width();

	for(int i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		if(existing_directory(full_path))
			printf(AC_B_BLUE"%s"AC_RESET, files[i]);
		else if(executable_file(full_path))
			printf(AC_B_GREEN"%s"AC_RESET, files[i]);
		else
			printf(AC_WHITE"%s"AC_RESET, files[i]);

		printf("  ");
	}
}

/* === START SORTING IN ALPHABETICAL ORDER === */
int compare(char *c, char *m)
{
	char current[80] = {0}, min[80] = {0};
	if(!print_begin_with_dot)
	{
		for(int i = 0; c[i] != '\0'; ++i)
			current[i] = tolower(c[i]);
		for(int i = 0; m[i] != '\0'; ++i)
			min[i] = tolower(m[i]);
	}
	else
	{
		for(int i = 0, j = 0; c[i] != '\0'; ++i)
		{
			if(c[i] == '.' && i == 0)
				continue;
			current[j++] = tolower(c[i]);
		}
		for(int i = 0, j = 0; m[i] != '\0'; ++i)
		{
			if(m[i] == '.' && i == 0)
				continue;
			min[j++] = tolower(m[i]);
		}
	}

	if(strcmp(current, min) < 0)
		return 1;
	return 0;
}

void sort_alphabetically(char *names[], size_t num)
{
	int min = 0;
	for(int i = 0; i < num - 1; ++i)
	{
		min = i;

		for(int j = i + 1; j < num; ++j)
		{
			if(compare(names[j], names[min]))
				min = j;
		}

		char *temp = names[i];
		names[i] = names[min];
		names[min] = temp;
	}
}
/* === END SORTING IN ALPHABETICAL ORDER === */

void directory_stream(char *dir)
{
	DIR *dp;
	struct dirent *dsp;
	char **names = NULL;
	size_t number_files = 0;

	if(!(dp = opendir(dir)))
		sys_error("opendir error");

	while( (dsp = readdir(dp)) != NULL)
	{
		if(dsp->d_name[0] == '.')
			if(print_begin_with_dot)
			{
				names = xrealloc(names, sizeof *names * (number_files + 1) );
				names[number_files] = xrealloc(names[number_files],
											   sizeof **names * (strlen(dsp->d_name) + 1));
				strcpy(names[number_files++], dsp->d_name);
			}
			else
				continue;
		else
		{
				names = xrealloc(names, sizeof *names * (number_files + 1) );
				names[number_files] = xrealloc(names[number_files],
											   sizeof **names * (strlen(dsp->d_name) + 1));
				strcpy(names[number_files++], dsp->d_name);
		}
	}

	sort_alphabetically(names, number_files);
	simple_print_file(dir, names, number_files);

	printf("\n");
}

void read_input(int argc, char *argv[])
{
	if(argc == 1 || argv[argc - 1][0] == '-')
	{
		char path[S_PATH];
		current_directory(path, S_PATH);
		directory_stream(path);
	}

	for(int i = argc - 1; i > 0; --i)
	{
		if(argv[i][0] == '-')
			break;
		if(!existing_directory(argv[i]))
			user_error("Nonexcistent directory %s", argv[i]);
		directory_stream(argv[i]);
	}
}

int main(int argc, char *argv[])
{
	char option;

	while( (option = getopt(argc, argv, OPTIONS)) > 0)
	{
		switch(option)
		{
			case 'a':
				print_begin_with_dot = true;
				break;
			case 'l':
				break;
			case '?':
				break;
		}
	}

	read_input(argc, argv);

	return 0;
}

