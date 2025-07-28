#include <stdbool.h>
#include <sys/types.h>
#include "grid.h"

extern Cell cells[ROW_LEN][ROW_LEN];
extern Cell answer[ROW_LEN][ROW_LEN];


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
void get_box(Cell box[ROW_LEN], int r, int c) {
    /* Figure out where the start of the current box is. */

    int start_r = r - r % 3, start_c = c - c % 3, count = 0, i, j;
    for (i = start_r; i < start_r + 3; i++) {
        for (j = start_c; j < start_c + 3; j++) {
            box[count] = cells[i][j];
            count++;
        }
    }
}


/* Gets row R from cells and puts it in ROW. */
void get_row(Cell row[ROW_LEN], int r) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        row[i] = cells[r][i];
    }
}


/* Gets column C from cells and puts it in col. */
void get_col(Cell col[ROW_LEN], int c) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        col[i] = cells[i][c];
    }
}


/* Checks if a Cell has value VALUE in a row, column or box. */
bool is_in_subset(int value, Cell subset[ROW_LEN], bool note) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        if (!note && subset[i].value == value) {
            return true;
        }
        /* Used by solver to see if any cells contains a certain note
         * which then ought to be removed. */
        if (subset[i].notes[value - 1]) {
            return true;
        }
    }
    return false;
}


/* Checks if a value can be placed in a given spot,
 * in terms of its environment (sudoku rules). */
bool is_allowed(int value, int r, int c) {
    Cell subset[ROW_LEN] = {};
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

