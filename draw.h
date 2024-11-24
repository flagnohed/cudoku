#ifndef DRAW_H_
#define DRAW_H_

/* Position in the sudoku while drawing. */
#define X 5
#define Y 3
#define MARGIN 3

/* Position right after the sudoku. */
#define MSG_POS Y + 2 * ROW_LEN + 1

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

#define CLEAR_LINE(y)   \
    {                   \
        move(y, 0);     \
        clrtoeol();     \
    }

#define REFRESH_0() \
    {               \
        move(0,0);  \
        refresh();  \
    }

#define OUTPUT_MSG(...)                         \
    {                                           \
        CLEAR_LINE(MSG_POS + 1);                \
        mvprintw(MSG_POS + 1, X, __VA_ARGS__);  \
    }

void draw_sudoku();
void init_cursor(int *y, int *x);
void write_cell(int *y, int *x, int ch);
void move_cursor(int *y, int *x, Direction_t dir);

#endif
