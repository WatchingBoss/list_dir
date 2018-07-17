#ifndef COMMON_H
#define COMMON_H

enum eST_MODE_VALUE
{
	IS_DIR,
	IS_EXE,
	IS_LNK,
	IS_PLAIN,
};

extern void user_error(char *e, ...);
extern void sys_error(char *e);

extern void current_directory(char *str, int);
extern int existing_directory(char *);

extern void *xrealloc(void *, size_t);

extern int get_terminal_width();

extern int st_mode_value(char *);

#endif
