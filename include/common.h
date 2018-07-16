#ifndef COMMON_H
#define COMMON_H

extern void user_error(char *e, ...);
extern void sys_error(char *e);
extern void current_directory(char *str, int);
extern int existing_directory(char *);
extern int executable_file(char *);
extern void *xrealloc(void *, size_t);
extern int get_terminal_width();
extern int find_middle(char *[], size_t);


#endif
