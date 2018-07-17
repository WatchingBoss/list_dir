#ifndef LIST_H
#define LIST_H

#include "common.h"

typedef enum{false, true} bool;

typedef struct sTable
{
	size_t sum_len, win_width, cols, lines;
} sTable; 

extern void check_line_filling(char *[], size_t, sTable *, int []);
extern void greatest_len_in_col(char *[], size_t, sTable *, int []);

#endif
