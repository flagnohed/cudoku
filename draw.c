#include <ncurses.h>
#include "grid.h"
#include "draw.h"

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
void write_cell(int *y, int *x, int ch) {
    if (ch < '1' || ch > '9') {
        OUTPUT_MSG("Could not write char %c (%d) to cell.", ch, ch);
        return;
    }
    
    int r = *y, c = *x;
    screen2cells(&r, &c);
    if (is_constant(r, c)) {
        OUTPUT_MSG("Cell is constant.");
        return;
    }

    int val = ch - '0';
    if (get_value(r, c) == val) {
        /* It's already there stupid. */
        return;
    }
    
    if (!is_allowed(val, r, c)) {
        OUTPUT_MSG("Writing %d at (%d, %d) would be a crime!", val, r, c);
        return;
    }
    
    set_value(val, r, c);
    cells2screen(&r, &c);
    *y = r;
    *x = c;
}


void move_cursor(int *y, int *x, Direction_t dir) {
    int r = *y, c = *x;
    screen2cells(&r, &c);
    
    switch (dir) {
        case DIR_UP:
            r -= r > 0 ? 1 : 0;
            c = get_closest_non_const(r, c);
            cells2screen(&r, &c);
            *y = r;
            *x = c;
            break;
        case DIR_DOWN:
            r += r < ROW_LEN - 1 ? 1 : 0;
            c = get_closest_non_const(r, c);
            cells2screen(&r, &c);
            *y = r;
            *x = c;
            break;
        case DIR_LEFT:
            for (int i = c - 1; i >= 0; i--) {
                if (!is_constant(r, i)) {
                    cells2screen(&r, &i);
                    *y = r;
                    *x = i;
                    break;
                }
            }
            break;
        case DIR_RIGHT:
            for (int i = c + 1; i < ROW_LEN; i++) {
                if (!is_constant(r, i)) {
                    cells2screen(&r, &i);
                    *y = r;
                    *x = i;
                    break;
                }
            }
            break;
        default:
            OUTPUT_MSG("move_cursor: unknown direction %d", dir);
            break;
    }
}


void init_cursor(int *y, int *x) {
    for (int r = 0; r < ROW_LEN; r++) {
        for (int c = 0; c < ROW_LEN; c++) {
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
    move(Y, X);
    
    for (int y = 0; y < ROW_LEN; y++) {
        
        for (int x = 0; x < ROW_LEN; x++) {
            printw("+---");
        }

        printw("+");
        move(Y + 2 * y + 1, X);

        for (int x = 0; x < ROW_LEN; x++) {
            /* Leave cell blank if no value, else convert value to char. */
            int value = cells[y][x].value;
            value = (value == 0 ? ' ' : value + '0');

            printw("|");
            printw(" %c ", value);
        }

        move(Y + 2 * y + 2, X);
    }
    
    for (int x = 0; x < ROW_LEN; x++) {
        printw("+---");
    }
    printw("+");
    REFRESH_0();
}
