/*
 * Small help functions
 */
#include "include/inc.h"

int get_terminal_width()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

size_t sum_length (char *files[], size_t num)
{
	size_t sum = 0;
	for(size_t i = 0; i < num; ++i)
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
	{
		sys_error("lstat in st_mode_value() error");
	}

	if(S_ISLNK(sb.st_mode))
		return IS_LNK;
	else if(S_ISDIR(sb.st_mode))
		return IS_DIR;
	else if(sb.st_mode & S_IXUSR)
		return IS_EXE;
	else
		return IS_PLAIN;
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

size_t size_of_file(char *path)
{
	struct stat sb;
	size_t size = 0;
	if(lstat(path, &sb))
		sys_error("lstat in size_of_file() error");

	size = sb.st_size;
	return size;
}


void get_file_info(const char *file, sIFLPF *info)
{
	struct stat sb;

	if(lstat(file, &sb))
		sys_error("lstat in get_file_info() error");

	memset(info->date, 0, 30);
	strftime(info->date, 30, "%b %d %I:%M", localtime(&(sb.st_mtime)));

	if(S_ISLNK(sb.st_mode))
	{
		info->value = IS_LNK;
		info->perm[0] = 'l';
	}
	else if(S_ISDIR(sb.st_mode))
	{
		info->value = IS_DIR;
		info->perm[0] = 'd';
	}
	else if(sb.st_mode & S_IXUSR)
	{
		info->value = IS_EXE;
		info->perm[0] = '-';
	}
	else
	{
		info->value = IS_PLAIN;
		info->perm[0] = '-';
	}

	if(sb.st_mode & S_IRUSR)
		info->perm[1] = 'r';
	else
		info->perm[1] = '-';
	if(sb.st_mode & S_IWUSR)
		info->perm[2] = 'w';
	else
		info->perm[2] = '-';
	if(sb.st_mode & S_IXUSR)
		info->perm[3] = 'x';
	else
		info->perm[3] = '-';

	if(sb.st_mode & S_IRGRP)
		info->perm[4] = 'r';
	else
		info->perm[4] = '-';
	if(sb.st_mode & S_IWGRP)
		info->perm[5] = 'w';
	else
		info->perm[5] = '-';
	if(sb.st_mode & S_IXGRP)
		info->perm[6] = 'x';
	else
		info->perm[6] = '-';

	if(sb.st_mode & S_IROTH)
		info->perm[7] = 'r';
	else
		info->perm[7] = '-';
	if(sb.st_mode & S_IWOTH)
		info->perm[8] = 'w';
	else
		info->perm[8] = '-';
	if(sb.st_mode & S_IXOTH)
		info->perm[9] = 'x';
	else
		info->perm[9] = '-';
}

void * xrealloc(void *ptr, size_t bytes)
{
	ptr = realloc(ptr, bytes);
	if(!ptr)
		sys_error("realloc error");
	return ptr;
}

void * xmalloc(size_t bytes)
{
	void *ptr = malloc(bytes);
	if(!ptr)
		sys_error("malloc error");
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
