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
#define AC_B_CYAN  "\x1b[96;1m"
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

size_t min_max_len_str(char *files[], size_t num, char flag)
{
	if(flag == 1)
	{
		size_t min = 0;
		for(int i = 0; i < num; ++i)
			if(strlen(files[i]) < min)
				min = strlen(files[i]);
		return min;
	}
	else if(flag == 2)
	{
		size_t max = 0;
		for(int i = 0; i < num; ++i)
			if(max < strlen(files[i]))
				max = strlen(files[i]);
		return max;
	}
	else
		printf("Invalid flag for min_max_len_str()\n");
	return 0;
}

size_t sum_length (char *files[], size_t num)
{
	size_t sum = 0;
	for(int i = 0; i < num; ++i)
		sum += (strlen(files[i]) + 2);

	return sum;
}

void simple_print_file(char *path, char *files[], size_t num)
{
	char full_path[S_PATH];
	size_t sum_len = sum_length(files, num);
	size_t win_width = get_terminal_width();
	size_t min_str_len = min_max_len_str(files, num, 1);
	size_t max_str_len = min_max_len_str(files, num, 2);
	int value;

	if(sum_len <= win_width)
		;		// Just print all in one line

	for(int i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);
		value = st_mode_value(full_path);

		switch(value)
		{
			case IS_DIR:
				printf(AC_B_BLUE"%s"AC_RESET, files[i]);
				break;
			case IS_EXE:
				printf(AC_B_GREEN"%s"AC_RESET, files[i]);
				break;
			case IS_LNK:
				printf(AC_B_CYAN"%s"AC_RESET, files[i]);
				break;
			case IS_PLAIN:
				printf(AC_WHITE"%s"AC_RESET, files[i]);
				break;
		}

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

	closedir(dp);
	for(int i = 0; i < number_files; ++i)
		free(names[i]);
	free(names);

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

