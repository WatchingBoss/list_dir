#ifndef COMMON_H
#define COMMON_H

enum eST_MODE_VALUE
{
	IS_DIR,
	IS_EXE,
	IS_LNK,
	IS_PLAIN,
};

typedef struct sIFLPF
{
	int value;
	char perm[10];
	struct passwd *owner_user;
	struct group *owner_group;
	char date[30];
} sIFLPF;

extern void user_error(char *e, ...);
extern void sys_error(char *e);

extern void current_directory(char *str, int);
extern int existing_directory(char *);

extern void * xrealloc(void *, size_t);
extern void *xmalloc(size_t);

extern int get_terminal_width();
extern size_t sum_length (char *[], size_t);

extern int st_mode_value(char *);
extern size_t number_of_files_in_dirs(char *);
extern size_t size_of_file(char *);
extern void get_file_info(char *, sIFLPF *);


#endif
