/*
 * List directory linux terminal utility
 */
#include "include/inc.h"

#define S_PATH 256
#define OPTIONS "alhkmg::"

/* === START GLOBAL VARIABLE === */
bool print_begin_with_dot = false;
bool print_like_list = false;
bool print_size_human = false;
bool print_size_kb = false;
bool print_size_mb = false;
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
			case 'h':
				print_size_human = true;
				break;
			case 'k':
				print_size_kb = true;
				break;
			case 'm':
				print_size_mb = true;
				break;
			case '?':
				break;
		}
	}

	read_input(argc, argv);

	return 0;
}

void find_num_of_files(char *path, char *files[], size_t num,
					   char number_of_files[][NUM_WIDTH])
{
	size_t num_of_files[num];
	char full_path[S_PATH];

	for(size_t i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		num_of_files[i] = number_of_files_in_dirs(full_path);
	}
	
	size_t width[num], max_width = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t w = 1;
		for(int j = num_of_files[i]; j >= 10; j /= 10)
			++w;
		if(w > max_width)
			max_width = w;

		width[i] = w;
	}

	for(size_t i = 0; i < num; ++i)
	{
		char blanks[NUM_WIDTH];
		memset(blanks, 0, sizeof blanks);
		for(int j = 0, b = max_width - width[i]; b != 0; --b)
			blanks[j++] = ' ';
		snprintf(number_of_files[i], NUM_WIDTH, "%s%ld", blanks, num_of_files[i]);
	}
}

void size_of_files(char *path, char *files[], size_t num, char files_size[][NUM_WIDTH])
{
	float sizes[num];
	char full_path[S_PATH];

	for(size_t i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		sizes[i] = size_of_file(full_path);
	}

	char human_size[num];
	if(print_size_human)
	{
		memset(human_size, 0, sizeof human_size);
		for(size_t i = 0; i < num; ++i)
			if(sizes[i] > 1000000000)
			{
				sizes[i] /= 1000000000;
				human_size[i] = 'g';
			}
			else if(sizes[i] > 1000000)
			{
				sizes[i] /= 1000000;
				human_size[i] = 'm';
			}
			else if(sizes[i] > 1000)
			{
				sizes[i] /= 1000;
				human_size[i] = 'k';
			}
	}
	else if(print_size_kb)
	{
		for(size_t i = 0; i < num; ++i)
			sizes[i] /= 1000;
	}
	else if(print_size_mb)
	{
		for(size_t i = 0; i < num; ++i)
			sizes[i] /= 1000000;
	}

	size_t width[num], max_width = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t w = 1;
		for(int j = sizes[i]; j >= 10; j /= 10)
			++w;
		if(w > max_width)
			max_width = w;

		width[i] = w;
	}

	for(size_t i = 0; i < num; ++i)
	{
		int j = 0;
		char blanks[NUM_WIDTH];
		memset(blanks, 0, sizeof blanks);
		for(int b = max_width - width[i]; b != 0; --b)
			blanks[j++] = ' ';
		if(!print_size_human && !print_size_kb && !print_size_mb)
			snprintf(files_size[i], NUM_WIDTH, "%s%.0f", blanks, sizes[i]);
		else if(print_size_human)
			snprintf(files_size[i], NUM_WIDTH, "%s%.1f%c", blanks, sizes[i],
					 human_size[i] == 'k' ? 'K' :
					 human_size[i] == 'm' ? 'M' :
					 human_size[i] == 'g' ? 'G' : 'B');
		else if(print_size_kb)
			snprintf(files_size[i], NUM_WIDTH, "%s%.1f%c", blanks, sizes[i], 'K');
		else if(print_size_mb)
			snprintf(files_size[i], NUM_WIDTH, "%s%.1f%c", blanks, sizes[i], 'M');
	}
}

void user_group_names(char *path, char *files[], size_t num,
					  char owner_u[][NUM_WIDTH], char owner_g[][NUM_WIDTH])
{
	char full_path[S_PATH];
	char o_user[num][NUM_WIDTH];
	char o_group[num][NUM_WIDTH];
	memset(o_user, 0, sizeof o_user);
	memset(o_group, 0, sizeof o_group);
	struct passwd *owner_user;
	struct group *owner_group;

	for(size_t i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);

		struct stat sb;

		if(lstat(full_path, &sb))
			sys_error("lstat in user_group_names() error");

		owner_user = getpwuid(sb.st_uid);
		owner_group = getgrgid(sb.st_gid);

		strncpy(o_user[i], owner_user->pw_name, NUM_WIDTH);
		strncpy(o_group[i], owner_group->gr_name, NUM_WIDTH);
	}
	
	size_t width_user[num], width_group[num], max_width_user = 0, max_width_group = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t user = 1, group = 1;
		for(size_t j = strlen(o_user[i]); j >= 1; --j)
			++user;
		for(size_t j = strlen(o_group[i]); j >= 1; --j)
			++group;
		if(user > max_width_user)
			max_width_user = user;
		if(group > max_width_group)
			max_width_group = group;

		width_user[i] = user;
		width_group[i] = group;
	}

	for(size_t i = 0; i < num; ++i)
	{
		char blanks_user[NUM_WIDTH], blanks_group[NUM_WIDTH];
		memset(blanks_user, 0, sizeof blanks_user);
		memset(blanks_group, 0, sizeof blanks_group);
		for(int j = 0, b = max_width_user - width_user[i]; b != 0; --b)
			blanks_user[j++] = ' ';
		for(int j = 0, b = max_width_group - width_group[i]; b != 0; --b)
			blanks_group[j++] = ' ';
		snprintf(owner_u[i], NUM_WIDTH, "%s%s", blanks_user, o_user[i]);
		snprintf(owner_g[i], NUM_WIDTH, "%s%s", blanks_group, o_group[i]);
	}
}

void list_print_file(char *path, char *files[], size_t num)
{
	char full_path[S_PATH];
	sIFLPF info;
	char num_of_files[num][NUM_WIDTH];
	char files_size[num][NUM_WIDTH];
	char owner_user[num][NUM_WIDTH];
	char owner_group[num][NUM_WIDTH];

	memset(owner_user, 0, sizeof owner_user);
	memset(owner_group, 0, sizeof owner_group);
	user_group_names(path, files, num, owner_user, owner_group);

	memset(num_of_files, 0, sizeof num_of_files);
	find_num_of_files(path, files, num, num_of_files);

	memset(files_size, 0, sizeof files_size);
	size_of_files(path, files, num, files_size);

	for(size_t i = 0; i < num; ++i)
	{
		memset(full_path, 0, S_PATH);
		memset(info.perm, 0, sizeof info.perm);

		snprintf(full_path, S_PATH, "%s/%s", path, files[i]);
		get_file_info(full_path, &info);

		for(int j = 0; j < 10; ++j)
			printf("%c", info.perm[j]);

		printf(" %s ", num_of_files[i]);
		printf("%s %s", owner_user[i], owner_group[i]);
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
			{
				printf(AC_B_CYAN"%s"AC_RESET, files[i]);
				char actual_path[S_PATH];
				memset(actual_path, 0, sizeof actual_path);
				char *ptr = realpath(full_path, actual_path);
				if(ptr == NULL)
					printf(" -> Unknown path");
				else
				{
					size_t actual_file_mode = st_mode_value(actual_path);

					switch(actual_file_mode)
					{
						case IS_DIR:
							printf(" -> "AC_B_BLUE"%s"AC_RESET, actual_path);
							break;
						case IS_EXE:
							printf(" -> "AC_B_GREEN"%s"AC_RESET, actual_path);
							break;
						case IS_PLAIN:
							printf(" -> "AC_WHITE"%s"AC_RESET, actual_path);
							break;
					}
				}
			} break;
			case IS_PLAIN:
				printf(AC_WHITE"%s"AC_RESET, files[i]);
				break;
		}

		putchar('\n');
	}
}

void check_line_filling(char *files[], size_t num, sTable *table, int width[])
{
	size_t len[table->lines];
	memset(len, 0, sizeof len);

	for(size_t line = 0; line < table->lines; ++line)
		for(size_t w = 0, l = 0; l < table->cols; w += table->lines, ++l)
		{
			if(w >= num)
				break;
			len[line] += (strlen(files[w]) + 2);
		}
	for(size_t i = 0; i < table->lines; ++i)
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
	for(size_t line = 0; line < table->lines; ++line)
		for(size_t c = 0, w = line; c < table->cols && w < num; ++c, w += table->lines)
		{
			int temp_len = strlen(files[w]) + 2;
			if(width[c] < temp_len)
				width[c] = temp_len;
		}
	size_t sum_max = 0;
	for(size_t i = 0; i < table->cols; ++i)
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

	for(size_t line = 0; line < table.lines; ++line)
	{
		for(size_t j = line, c = 0; c < table.cols && j < num; j += table.lines, ++c)
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
				for(size_t i = width[c] - temp_len; i != 0; --i)
					putchar(' ');
			else
				for(size_t i = 0; i < 2; ++i)
					putchar(' ');
		}
		putchar('\n');
	}
}

void directory_stream(char *dir)
{
	DIR *dp;
	struct dirent *dsp;
	char **names = malloc(sizeof(char *)), **temp = NULL, *temp_p = NULL;
	size_t number_files = 0;

	if(!(dp = opendir(dir)))
		sys_error("opendir error");

	while( (dsp = readdir(dp)) != NULL)
	{
		if(dsp->d_name[0] == '.' && !print_begin_with_dot)
				continue;
		else
		{
				temp = xrealloc(names, sizeof *names * (number_files + 1) );
				if(temp)
					names = temp;
				else
					sys_error("xrealloc **names error");
				temp_p = xmalloc(sizeof **names * (strlen(dsp->d_name) + 1));
				if(temp)
					names[number_files] = temp_p;
				else
					sys_error("xrealloc *names error");
				strcpy(names[number_files++], dsp->d_name);
		}
	}

	sort_alphabetically(names, number_files);

	if(print_like_list)
		list_print_file(dir, names, number_files);
	else
		simple_print_file(dir, names, number_files);

	closedir(dp);
	for(size_t i = 0; i < number_files; ++i)
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

	for(size_t i = argc - 1; i > 0; --i)
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
		for(size_t i = 0; c[i] != '\0'; ++i)
			current[i] = tolower(c[i]);
		for(size_t i = 0; m[i] != '\0'; ++i)
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
	size_t min = 0;
	for(size_t i = 0; i < num - 1; ++i)
	{
		min = i;

		for(size_t j = i + 1; j < num; ++j)
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

