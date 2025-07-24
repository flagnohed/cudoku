#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include <sys/types.h>

Cell cells[ROW_LEN][ROW_LEN] = {};
Cell answer[ROW_LEN][ROW_LEN] = {};


/* Reads a grid from file FNAME. */
void read_grid(const char *fname, bool is_answer) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int r = 0, c;
    fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Could not open %s.\n", fname);
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        for (c = 0; c < ROW_LEN; c++) {
            Cell cell;
            cell.row = r;
            cell.col = c;
            cell.value = line[c] - '0';
            cell.is_constant = cell.value != 0;
            if (is_answer)
                answer[r][c] = cell;
            else
                cells[r][c] = cell;
        }
        r++;
    }
    fclose(fp);
}


/* Returns the cell at the given coordinates. */
static Cell *get_cell(int r, int c) {
    return &cells[r][c];
}


/* Returns true if cell was given at the start. */
bool is_constant(int r, int c) {
    return get_cell(r, c)->is_constant;
}


/* Get the value of the cell at (R, C). */
int get_value(int r, int c) {
    return get_cell(r, c)->value;
}


/* Either set the value or note a value in the cell at (r, c).
 * If we already have noted or set the value at this cell, we
 * interpret it as a toggle, i.e., set it to 0 instead. */
void set_value(int val, int r, int c, bool note) {
    Cell *cell = get_cell(r, c);
    if (note) {
        /* Note value at current cell, or remove the note it value is reentered. */
        cell->notes[val - 1] = (cell->notes[val - 1] ? 0 : val);
        return;
    }
    cell->value = (cell->value ? 0 : val);
}


/* If we are moving vertically, and the new
 * cell is constant, figure out where the
 * closest non-constant cell is horizontally. */
int get_closest_non_const(int r, int c) {
    int x = 0, l_delta = ROW_LEN, r_delta = ROW_LEN;

    if (!get_cell(r, c)->is_constant) {
        return c;
    }

    for (x = c; x >= 0; x--) {
        if (!get_cell(r, x)->is_constant) {
            /* Found a non-const cell to the left. */
            l_delta = c - x;
            break;
        }
    }

    for (x = c + 1; x < ROW_LEN; x++) {
        if (!get_cell(r, x)->is_constant) {
            /* Found a non-const cell to the right. */
            r_delta = x - c;
            break;
        }
    }
    /* Adjust x with the smallest delta. */
    x = r_delta >= l_delta ? c - l_delta : c + r_delta;
    return x >= 0 && x < ROW_LEN ? x : c;
}


/* Prints a row, column or box (last two are printed as rows). */
void print_subset(Cell subset[ROW_LEN]) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        printf("%d", subset[i].value);
    }
    printf("\n");
}


/* Prints a given grid. */
void print_grid(Cell grid[ROW_LEN][ROW_LEN]) {
    int r;
    for (r = 0; r < ROW_LEN; r++) {
        print_subset(grid[r]);
    }
}


/* Prints CELLS and ANSWER. */
void print_grids() {
    printf("Current grid:\n");
    print_grid(cells);
    printf("\nAnswer grid:\n");
    print_grid(answer);
}


/* Use [ROW_LEN] here instead of [BOX_LEN][BOX_LEN].
 * This is because it will be easier to traverse it.
 * We can still access the coordinates for each cell
 * since they keep track on it themselves.
 */
static void get_box(Cell box[ROW_LEN], int r, int c) {
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
static void get_row(Cell row[ROW_LEN], int r) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        row[i] = cells[r][i];
    }
}


/* Gets column C from cells and puts it in col. */
static void get_col(Cell col[ROW_LEN], int c) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        col[i] = cells[i][c];
    }
}


/* Checks if a Cell has value VALUE in a row, column or box. */
static bool is_in_subset(int value, Cell subset[ROW_LEN]) {
    int i;
    for (i = 0; i < ROW_LEN; i++) {
        if (subset[i].value == value) {
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
    if (is_in_subset(value, subset)) {
        OUTPUT_MSG("Found %d in row %d\n", value, r);
        return false;
    }
    get_col(subset, c);
    if (is_in_subset(value, subset)) {
        OUTPUT_MSG("Found %d in column %d\n", value, c);
        return false;
    }
    get_box(subset, r, c);
    if (is_in_subset(value, subset)) {
        OUTPUT_MSG("Found %d in box containing index (%d, %d)\n", value, r, c);
        return false;
    }
    return true;
}


/* Checks if a CELL at (R, C) is correct. */
static bool is_cell_correct(int r, int c) {
    return get_cell(r, c)->value == answer[r][c].value;
}


/* Checks if CELLS equals ANSWER. */
bool is_complete() {
    int i, j;
    for (i = 0; i < ROW_LEN; i++) {
        for (j = 0; j < ROW_LEN; j++) {
            if (is_cell_correct(i, j))
                return false;
        }
    }
    return true;
}

