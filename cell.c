#include <stdbool.h>
#include <sys/types.h>
#include "cell.h"


extern Cell cells[ROW_LEN][ROW_LEN];
extern Cell answer[ROW_LEN][ROW_LEN];


/* Convert CELLS indexes to screen coordinates.
 * Used in main.c to initiate cursor. */
void cells2screen(int *r, int *c) {
    *r = Y + (*r * 2) + 1;
    *c = X + (*c * 4) + 2;
}


/* Converts screen coordinates to CELLS indexes. */
void screen2cells(int *y, int *x) {
    *y = (*y - Y - 1) / 2;
    *x = (*x - X - 2) / 4;
}


/* Writes CH (value as char) at the correct position in CELLS.
 * We complain if CH is invalid or if we already have a
 * constant value there (i.e. not user added). */
void write_cell(int r, int c, int ch, bool note) {
    int val;
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
}


/* Either set the value or note a value in the cell at (r, c).
 * If we already have noted or set the value at this cell, we
 * interpret it as a toggle, i.e., set it to 0 instead. */
void set_value(int val, int r, int c, bool note) {
    int i;
    Cell *cell = &cells[r][c];
    if (note) {
        /* Note value at current cell, or remove the note it value is reentered. */
        cell->notes[val - 1] = (cell->notes[val - 1] ? 0 : val);
        return;
    }
    /* Erasing by reentering value is handled earlier in the call chain. */
    if ((cell->value = val) == 0) {
        return;
    }
    /* If we are not erasing, i.e., we are adding a value != 0 to the cell,
     * we want to remove all notes from the cell. */
    for (i = 0; i < ROW_LEN; i++) {
        cell->notes[i] = 0;
    }
}


/* Use [ROW_LEN] here instead of [BOX_LEN][BOX_LEN].
 * This is because it will be easier to traverse it.
 * We can still access the coordinates for each cell
 * since they keep track on it themselves.
 */
void get_box(Cell *box[ROW_LEN], int r, int c) {
    /* Figure out where the start of the current box is. */
    int start_r = r - r % 3, start_c = c - c % 3, count = 0, i, j;
    for (i = start_r; i < start_r + 3; i++) {
        for (j = start_c; j < start_c + 3; j++) {
            box[count] = &cells[i][j];
            count++;
        }
    }
}


/* Gets row R from cells and puts it in ROW. */
void get_row(Cell *row[ROW_LEN], int r) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        row[i] = &cells[r][i];
    }
}


/* Gets column C from cells and puts it in col. */
void get_col(Cell *col[ROW_LEN], int c) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        col[i] = &cells[i][c];
    }
}


/* Checks if a Cell has value VALUE in a row, column or box. */
bool is_in_subset(int value, Cell *subset[ROW_LEN], bool note) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        if (!note && subset[i]->value == value) {
            return true;
        }
        /* Used by solver to see if any cells contains a certain
         * note which then ought to be removed. */
        if (note && subset[i]->notes[value - 1]) {
            return true;
        }
    }
    return false;
}


/* Checks if a value can be placed in a given spot,
 * in terms of its environment (sudoku rules). */
bool is_allowed(int value, int r, int c) {
    if (cells[r][c].is_constant) {
        OUTPUT_MSG("Cell at (%d, %d) is constant (%d)", r, c, value);
        return false;
    }
    Cell *subset[ROW_LEN] = {};
    get_row(subset, r);
    if (is_in_subset(value, subset, false)) {
        OUTPUT_MSG("Found %d in row %d\n", value, r);
        return false;
    }
    get_col(subset, c);
    if (is_in_subset(value, subset, false)) {
        OUTPUT_MSG("Found %d in column %d\n", value, c);
        return false;
    }
    get_box(subset, r, c);
    if (is_in_subset(value, subset, false)) {
        OUTPUT_MSG("Found %d in box containing index (%d, %d)\n", value, r, c);
        return false;
    }
    return true;
}


/* Checks if a CELL at (R, C) is correct. */
static bool is_cell_correct(int r, int c) {
    return cells[r][c].value == answer[r][c].value;
}


/* Checks if CELLS equals ANSWER. */
bool is_complete() {
    int i, j;
    for (i = 0; i < ROW_LEN; i++) {
        for (j = 0; j < ROW_LEN; j++) {
            if (!is_cell_correct(i, j))
                return false;
        }
    }
    return true;
}

