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


void write_cell(int v, int r, int c, bool note) {
    Cell *cell = &cells[r][c];
    if (cell->is_constant) {
        OUTPUT_MSG("Cell is constant.");
        return;
    }
    if (cell->value == v) {
        /* Reentering the already existing value
           is interpreted as erasing. */
        v = 0;
    }
    else if (!is_allowed(v, r, c)) {
        /* is_allowed() handles "error" message, so just return. */
        return;
    }
    set_value(v, r, c, note);
}


/* Either set the value or note a value in the cell at (r, c).
 * If we already have noted or set the value at this cell, we
 * interpret it as a toggle, i.e., set it to 0 instead. */
void set_value(int v, int r, int c, bool note) {
    int i;
    Cell *cell = &cells[r][c];
    if (note) {
        /* Note value at current cell, or remove the note it value is reentered. */
        cell->notes[v - 1] = (cell->notes[v - 1] ? 0 : v);
        return;
    }
    if (!(cell->value = v)) {
        /* Erasing by reentering value is handled earlier in the call chain. */
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
    int start_r = r - r % BOX_LEN;
    int start_c = c - c % BOX_LEN;
    int i, j, subset_idx;
    for (i = start_r; i < start_r + BOX_LEN; i++) {
        for (j = start_c; j < start_c + BOX_LEN; j++) {
            subset_idx = (i - start_r) * BOX_LEN + (j - start_c);
            box[subset_idx] = &cells[i][j];
        }
    }
}


/* Gets row R from cells and puts it in ROW. */
static void get_row(Cell *row[ROW_LEN], int r) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        row[i] = &cells[r][i];
    }
}


/* Gets column C from cells and puts it in col. */
static void get_col(Cell *col[ROW_LEN], int c) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        col[i] = &cells[i][c];
    }
}


/* Checks if a Cell has value VALUE in a row, column or box. */
static bool is_in_subset(int v, Cell *subset[ROW_LEN], bool note) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        if (!note && subset[i]->value == v) {
            return true;
        }
        /* Used by solver to see if any cells contains a certain
         * note which then ought to be removed. */
        if (note && subset[i]->notes[v - 1]) {
            return true;
        }
    }
    return false;
}


/* Checks if a value can be placed in a given spot,
 * in terms of its environment (sudoku rules). */
bool is_allowed(int v, int r, int c) {
    Cell *subset[ROW_LEN];
    if (cells[r][c].is_constant) {
        OUTPUT_MSG("Cell at (%d, %d) is constant (%d)", r, c, v);
        return false;
    }
    get_row(subset, r);
    if (is_in_subset(v, subset, false)) {
        OUTPUT_MSG("Found %d in row %d\n", v, r);
        return false;
    }
    get_col(subset, c);
    if (is_in_subset(v, subset, false)) {
        OUTPUT_MSG("Found %d in column %d\n", v, c);
        return false;
    }
    get_box(subset, r, c);
    if (is_in_subset(v, subset, false)) {
        OUTPUT_MSG("Found %d in box containing index (%d, %d)\n", v, r, c);
        return false;
    }
    return true;
}


static bool is_subset_solved(Cell *subset[ROW_LEN]) {
    int i, cur_idx;
    int seen_values[ROW_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (i = 0; i < ROW_LEN; i++) {
        if (subset[i]->value == 0) {
            return false;
        }
        cur_idx = subset[i]->value - 1;
        if (seen_values[cur_idx]) {
            /* We now have multiple instances of the same value
             * in this subset, which is not allowed! */
            return false;
        }
        /* Does not matter what we put here (as long as it is
         * not zero of course) since seen_values' values are
         * just used as booleans in practice. */
        seen_values[cur_idx] = 1;
    }
    /* At last, check if we have missed any value. */
    for (i = 0; i < ROW_LEN; i++) {
        if (!seen_values[subset[i]->value - 1]) {
            return false;
        }
    }
    return true;
}


/* Returns true if every row and every column is solved. */
bool is_complete() {
    int i;
    Cell *subset[ROW_LEN];
    for (i = 0; i < ROW_LEN; i++) {
        get_row(subset, i);
        if (!is_subset_solved(subset)) {
            return false;
        }
        get_col(subset, i);
        if (!is_subset_solved(subset)) {
            return false;
        }
    }
    return true;
}
