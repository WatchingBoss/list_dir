/*
 * List directory linux terminal utility
 */
#include "include/inc.h"

#define S_PATH 256
#define OPTIONS "al::"

/* === START GLOBAL VARIABLE === */
bool print_begin_with_dot = false;
bool print_like_list = false;
/* === END GLOBAL VARIABLE   === */

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
				print_like_list = true;
				break;
			case '?':
				break;
		}
	}

	read_input(argc, argv);

	return 0;
}

size_t number_of_files_in_dirs(char *d)
{
	DIR *dir;
	struct dirent *dp;
	size_t num_of_files = 0;
	if( !(dir = opendir(d)) )
		return 1;

	while( (dp = readdir(dir)) != NULL)
		if( strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
			++num_of_files;
	
	return num_of_files;
}

#define NUM_WIDTH 20
void find_width_for_num_of_files(char *path, char *files[], size_t num,
								 char number_of_files[][NUM_WIDTH])
{
	size_t num_of_files[num];
	char full_path[S_PATH];

	for(int i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		num_of_files[i] = number_of_files_in_dirs(full_path);
	}
	
	size_t width[num], max_width = 0;
	for(int i = 0; i < num; ++i)
	{
		int w = 1;
		for(int j = num_of_files[i]; j > 10; j /= 10)
			++w;
		if(w > max_width)
			max_width = w;

		width[i] = w;
	}

	for(int i = 0; i < num; ++i)
	{
		int j = 0;
		char blanks[NUM_WIDTH];
		memset(blanks, 0, sizeof blanks);
		for(int b = max_width - width[i]; b != 0; --b)
			blanks[j++] = ' ';
		snprintf(number_of_files[i], NUM_WIDTH, "%s%ld", blanks, num_of_files[i]);
	}
}

size_t size_of_file(char *path)
{
	struct stat sb;
	size_t size = 0;
	if(stat(path, &sb))
		sys_error("fstat in size_of_file error");

	size = sb.st_size;
	return size;
}

void size_of_files(char *path, char *files[], size_t num, char files_size[][NUM_WIDTH])
{
	size_t sizes[num];
	char full_path[S_PATH];

	for(int i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		sizes[i] = size_of_file(full_path);
	}
	
	size_t width[num], max_width = 0;
	for(int i = 0; i < num; ++i)
	{
		int w = 1;
		for(int j = sizes[i]; j > 10; j /= 10)
			++w;
		if(w > max_width)
			max_width = w;

		width[i] = w;
	}

	for(int i = 0; i < num; ++i)
	{
		int j = 0;
		char blanks[NUM_WIDTH];
		memset(blanks, 0, sizeof blanks);
		for(int b = max_width - width[i]; b != 0; --b)
			blanks[j++] = ' ';
		snprintf(files_size[i], NUM_WIDTH, "%s%ld", blanks, sizes[i]);
	}
}

void list_print_file(char *path, char *files[], size_t num)
{
	char full_path[S_PATH];
	sIFLPF info;
	char num_of_files[num][NUM_WIDTH];
	char files_size[num][NUM_WIDTH];

	memset(num_of_files, 0, sizeof num_of_files);
	find_width_for_num_of_files(path, files, num, num_of_files);

	memset(files_size, 0, sizeof files_size);
	size_of_files(path, files, num, files_size);

	for(int i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		memset(info.perm, 0, sizeof info.perm);

		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);
		get_file_info(full_path, &info);

		for(int j = 0; j < 10; ++j)
			printf("%c", info.perm[j]);

		printf(" %s ", num_of_files[i]);
		printf("%s %s", info.owner_user->pw_name, info.owner_group->gr_name);
		printf(" %s ", files_size[i]);
		printf("%s ", info.date);

		switch(info.value)
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

		putchar('\n');
	}
}

void check_line_filling(char *files[], size_t num, sTable *table, int width[])
{
	int len[table->lines];
	memset(len, 0, sizeof len);

	for(int line = 0; line < table->lines; ++line)
		for(int w = 0, l = 0; l < table->cols; w += table->lines, ++l)
		{
			if(w >= num)
				break;
			len[line] += (strlen(files[w]) + 2);
		}
	for(int i = 0; i < table->lines; ++i)
		if(table->win_width < len[i])
		{
			table->cols -= 1;
			table->lines = num / table->cols;
			check_line_filling(files, num, table, width);
		}
	if(table->lines == 1)
		table->cols += 1;
	if(table->cols * table->lines < num)
		table->lines += 1;

	greatest_len_in_col(files, num, table, width);
}

void greatest_len_in_col(char *files[], size_t num, sTable *table, int width[])
{
	for(int line = 0; line < table->lines; ++line)
		for(int c = 0, w = line; c < table->cols && w < num; ++c, w += table->lines)
		{
			int temp_len = strlen(files[w]) + 2;
			if(width[c] < temp_len)
				width[c] = temp_len;
		}
	size_t sum_max = 0;
	for(int i = 0; i < table->cols; ++i)
		sum_max += width[i];
	if(sum_max > table->win_width)
	{
		table->cols -= 1;
		check_line_filling(files, num, table, width);
	}
}

void simple_print_file(char *path, char *files[], size_t num)
{
	sTable table;
	table.sum_len = sum_length(files, num);
	table.win_width = get_terminal_width();
	table.cols = table.win_width / ((table.sum_len / num) * 2);
	table.lines = num / table.cols;

	char full_path[S_PATH];
	int value = 0;

	int width[table.cols + 1];
	memset(width, 0, sizeof width);

	check_line_filling(files, num, &table, width);

	for(int line = 0; line < table.lines; ++line)
	{
		for(int j = line, c = 0; c < table.cols && j < num; j += table.lines, ++c)
		{
			memset(full_path, 0, S_PATH);
			snprintf(full_path, S_PATH, "%s/%s", path, files[j]);
			value = st_mode_value(full_path);

			switch(value)
			{
				case IS_DIR:
					printf(AC_B_BLUE"%s"AC_RESET, files[j]);
					break;
				case IS_EXE:
					printf(AC_B_GREEN"%s"AC_RESET, files[j]);
					break;
				case IS_LNK:
					printf(AC_B_CYAN"%s"AC_RESET, files[j]);
					break;
				case IS_PLAIN:
					printf(AC_WHITE"%s"AC_RESET, files[j]);
					break;
			}
			int temp_len = strlen(files[j]);
			if(temp_len < width[c])
				for(int i = width[c] - temp_len; i != 0; --i)
					putchar(' ');
			else
				for(int i = 0; i < 2; ++i)
					putchar(' ');
		}
		putchar('\n');
	}
}

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

	if(print_like_list)
		list_print_file(dir, names, number_files);
	else
		simple_print_file(dir, names, number_files);

	closedir(dp);
	for(int i = 0; i < number_files; ++i)
		free(names[i]);
	free(names);
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

