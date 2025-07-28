#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"
#include "draw.h"

extern Cell cells[ROW_LEN][ROW_LEN];


/* Convert CELLS indexes to screen coordinates.
 * Used in main.c to initiate cursor. */
void cells2screen(int *r, int *c) {
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
    Cell *cell = &cells[r][c];
    if (cell->is_constant) {
        OUTPUT_MSG("Cell is constant.");
        return;
    }
    val = ch - '0';

    if (cell->value == val) {
        /* Reentering the already existing value
           is interpreted as erasing. */
        val = 0;
        OUTPUT_MSG("Cell value already there, erasing instead.")
    }
    else if (!is_allowed(val, r, c)) {
        return;
    }

    set_value(val, r, c, note);
    cells2screen(&r, &c);
    *y = r;
    *x = c;
}


/* Output all the notes for the current cell. */
void show_notes(int r, int c) {
    /* "Notes: " + "x, " * 9 + \0 = 7 + 27 + 1 = 35. */
    char note_str[35], tmp_str[4];
    int note_idx, cur_note;
    sprintf(note_str, "Notes: ");
    for (note_idx = 0; note_idx < ROW_LEN - 1; note_idx++) {
        cur_note = cells[r][c].notes[note_idx];
        if (cur_note) {
            sprintf(tmp_str, "%d, ", cur_note);
            strncat(note_str, tmp_str, 4);
        }
    }
    OUTPUT_MSG("%s", note_str);
}


void move_cursor(int *y, int *x, Direction_t dir) {
    int r = *y, c = *x;
    screen2cells(&r, &c);
    switch (dir) {
        case DIR_UP:
            r -= (r > 0 ? 1 : 0);
            break;
        case DIR_LEFT:
            c -= (c > 0 ? 1 : 0);
            break;
        case DIR_DOWN:
            r += (r < ROW_LEN - 1 ? 1 : 0);
            break;
        case DIR_RIGHT:
            c += (c < ROW_LEN - 1 ? 1 : 0);
            break;
        default:
            /* Should never happen! */
            OUTPUT_MSG("move_cursor: unknown direction %d", dir);
            exit(EXIT_FAILURE);
    }
    show_notes(r, c);
    cells2screen(&r, &c);
    *y = r;
    *x = c;
}


/* Uses ncurses to draw CELLS. Using bold text to print
 * out the 9 in the grid (1 box = 3x3 cells). */
void draw_sudoku() {
    int x, y;
    char value;
    move(Y, X);
    Cell *cell;

    for (y = 0; y < ROW_LEN; y++) {

        if (y % 3 == 0)
            attron(A_BOLD);
        for (x = 0; x < ROW_LEN; x++)
            printw("+---");
        printw("+");

        /* Top border done, now move down to values. */
        move(Y + 2 * y + 1, X);
        attroff(A_BOLD);

        for (x = 0; x < ROW_LEN; x++) {
            cell = &cells[y][x];
            if (x % 3 == 0)
                attron(A_BOLD);
            printw("|");
            attroff(A_BOLD);
            value = (char) cell->value + '0';
            if (value == '0')
                /* Don't print out 0 at the empty cells. */
                value = ' ';

            if (cell->is_constant)
                attron(A_BOLD);
            printw(" %c ", value);

            attroff(A_BOLD);
        }
        attron(A_BOLD);
        printw("|");
        move(Y + 2 * y + 2, X);
        attroff(A_BOLD);
    }
    for (x = 0; x < ROW_LEN; x++)
        printw("+---");
    printw("+");
    attroff(A_BOLD);
    REFRESH_0();
}
