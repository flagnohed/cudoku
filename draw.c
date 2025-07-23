#include <ncurses.h>
#include "grid.h"
#include "draw.h"
#include <stdlib.h>

extern Cell cells[ROW_LEN][ROW_LEN];


/* Convert CELLS indexes to screen coordinates. */
static void cells2screen(int *r, int *c) {
    *r = Y + (*r * 2) + 1;
    *c = X + (*c * 4) + 2;
}


/* Converts screen coordinates to CELLS indexes. */
static void screen2cells(int *y, int *x) {
    *y = (*y - Y - 1) / 2;
    *x = (*x - X - 2) / 4;
}


/* Writes CH (value as char) at the correct position in CELLS.
 * We complain if CH is invalid or if we already have a
 * constant value there (i.e. not user added). */
void write_cell(int *y, int *x, int ch, bool note) {
    int val, r, c;
    r = *y;
    c = *x;
    screen2cells(&r, &c);
    if (is_constant(r, c)) {
        OUTPUT_MSG("Cell is constant.");
        return;
    }
    val = ch - '0';

    if (!is_allowed(val, r, c)) { return; }
    if (get_value(r, c) == val) {
        /* Reentering the already existing value
           is interpreted as erasing. */
        val = 0;
    }
    set_value(val, r, c, note);
    cells2screen(&r, &c);
    *y = r;
    *x = c;
}


void move_cursor(int *y, int *x, Direction_t dir) {
    int r = *y, c = *x, i;
    screen2cells(&r, &c);
    switch (dir) {
        case DIR_UP:
            for (i = r - 1; i >= 0; i--) {
                if (is_constant(i, c))
                    continue;
                cells2screen(&i, &c);
                *y = i;
                *x = c;
                break;
            }
            break;
        case DIR_LEFT:
            for (i = c - 1; i >= 0; i--) {
                if (is_constant(r, i))
                    continue;
                cells2screen(&r, &i);
                *y = r;
                *x = i;
                break;
            }
            break;
        case DIR_DOWN:
            for (i = r + 1; i < ROW_LEN; i++) {
                if (is_constant(i, c))
                    continue;
                cells2screen(&i, &c);
                *y = i;
                *x = c;
                break;
            }
            break;
        case DIR_RIGHT:
            for (i = c + 1; i < ROW_LEN; i++) {
                if (is_constant(r, i))
                    continue;
                cells2screen(&r, &i);
                *y = r;
                *x = i;
                break;
            }
            break;
        default:
            /* Should never happen! */
            OUTPUT_MSG("move_cursor: unknown direction %d", dir);
            break;
    }
}


void init_cursor(int *y, int *x) {
    int r, c;
    for (r = 0; r < ROW_LEN; r++) {
        for (c = 0; c < ROW_LEN; c++) {
            if (!is_constant(r, c)) {
                cells2screen(&r, &c);
                *y = r;
                *x = c;
                return;
            }
        }
    }
    *y = 0;
    *x = 0;
}


/* Uses ncurses to draw CELLS. */
void draw_sudoku() {
    int x, y;
    char value;
    move(Y, X);
    for (y = 0; y < ROW_LEN; y++) {
        for (x = 0; x < ROW_LEN; x++)
            printw("+---");

        printw("+");
        move(Y + 2 * y + 1, X);
        for (x = 0; x < ROW_LEN; x++) {
            /* Leave cell blank if no value, else convert
               value to char. */
            value = (char) cells[y][x].value + '0';
            if (value == '0')
                value = ' ';  /* Don't print out 0 at the empty cells. */
            printw("|");
            if (cells[y][x].is_constant)
                attron(A_BOLD);
            printw(" %c ", value);
            attroff(A_BOLD);
        }
        printw("|");
        move(Y + 2 * y + 2, X);
    }
    for (x = 0; x < ROW_LEN; x++)
        printw("+---");
    printw("+");
    REFRESH_0();
}
