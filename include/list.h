#ifndef LIST_H
#define LIST_H

#include "common.h"

/* START COLORS */
#define AC_B_RED   "\x1b[91;1m"
#define AC_B_GREEN "\x1b[92;1m"
#define AC_B_BLUE  "\x1b[94;1m"
#define AC_B_CYAN  "\x1b[96;1m"
#define AC_WHITE   "\x1b[97;1m"
#define AC_RESET   "\x1b[0m"
/* END COLORS   */

#define NUM_WIDTH 20

typedef enum{false, true} bool;

typedef struct sTable
{
	size_t sum_len, win_width, cols, lines;
} sTable; 

extern void check_line_filling(char *[], size_t, sTable *, int []);
extern void greatest_len_in_col(char *[], size_t, sTable *, int []);
extern void simple_print_file(char *, char *[], size_t);

extern void find_num_of_files(char *, char *[], size_t, char[][NUM_WIDTH]);
extern void size_of_files(char *, char *[], size_t, char[][NUM_WIDTH]);
extern void list_print_file(char *, char *[], size_t);

extern int compare(char *, char *);
extern void sort_alphabetically(char *[], size_t);

extern void directory_stream(char *);
extern void read_input(int, char *[]);


#endif
