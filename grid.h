#ifndef GRID_H_
#define GRID_H_

#include <ncurses.h>

#define BOX_LEN 3
#define ROW_LEN BOX_LEN * BOX_LEN

/* Position in the sudoku while drawing. */
#define X 5
#define Y 3
#define MARGIN 3

/* Position right after the sudoku. */
#define MSG_POS Y + 2 * ROW_LEN + 1

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

typedef struct {
    int value;
    int row, col;
    bool is_constant;   /* True if cell not hidden at the start. */
    int notes[9];
}   Cell;

int get_closest_non_const(int r, int c);
int get_value(int r, int c);
bool is_allowed(int value, int r, int c);
bool is_constant(int r, int c);
void read_grid (const char *fname, bool is_answer);
void set_value(int val, int r, int c, bool note);
void print_grids();
void print_subset(Cell subset[ROW_LEN]);


#endif
