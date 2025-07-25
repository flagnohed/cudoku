#ifndef DRAW_H_
#define DRAW_H_

#include <stdbool.h>

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
}   Direction_t;

/* Special keys. */
#define KEY_UARR 259
#define KEY_DARR 258
#define KEY_LARR 260
#define KEY_RARR 261
#define KEY_CTRLC 3

typedef enum {
    COL_NORM,
    COL_BOLD,
    COL_SOFT,
    COL_FCOL,
    COL_NFCOL,
}   Color_t;

void draw_sudoku();
void write_cell(int *y, int *x, int ch, bool note);
void move_cursor(int *y, int *x, Direction_t dir);

#endif
