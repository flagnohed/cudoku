#include "cell.h"


#define MAX_VAL 9


typedef enum {
    BOX,
    COLUMN,
    ROW,
}   Subset_t;

extern Cell cells[ROW_LEN][ROW_LEN];


/* This function should only be called if all cells have been correctly noted.
 * Returns true if the given cell only has a single note. */
bool is_obvious_single(int r, int c) {
    int i, count = 0;
    for (i = 0; i < ROW_LEN; i++) {
        /* Count the number of notes on this cell. */
        if (cells[r][c].notes[i])
            count++;
    }
    return count == 1;
}


/* For a subset, check if we have one single empty cell.
 * Returns 0 if no last free cell found, or value between 1-9
 * if found. */
int last_free_cell(Cell subset[ROW_LEN]) {
    int i, found_value, empty_cells = 0;
    int unseen[ROW_LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (i = 0; i < ROW_LEN; i++) {
        found_value = subset[i].value;
        if (found_value == 0)
            empty_cells++;
        else
            unseen[found_value - 1] = 0;
    }
    if (empty_cells != 1)
        return 0;

    for (i = 0; i < ROW_LEN; i++) {
        if (unseen[i])
            return unseen[i];
    }

    /* Should never reach this. */
    return 0;
}


/* For the given coordinates, determine if there is only one possible value
 * that can go here. Returns that value if possible, else 0. */
int last_remaining_cell(int r, int c) {
    int count = 0, last_value = 0, v;
    for (v = 1; v <= MAX_VAL; v++) {
        if (is_allowed(v, r, c)) {
            count++;
            last_value = v;
        }
    }
    return (count == 1 ? last_value : 0);
}


/* Note all possible values of this cell. */
void note_possible_values(int r, int c) {
    int v;
    for (v = 1; v <= MAX_VAL; v++) {
        if (is_allowed(v, r, c)) {
            set_value(v, r, c, true);
        }
    }
}


/* This function does not use get_{row, col, box}
 * because those helper functions give copies of that subset.
 * In this solver specific function we need to manipulate
 * these places in the actual cells grid.
 * @todo: use pointers in get_* ? */
void remove_notes(int v, int r, int c) {
    int i, *note;
    Cell *box[ROW_LEN];
    /* First check notes in the same row and column. */
    for (i = 0; i < ROW_LEN; i++) {
        if ((note = &cells[r][i].notes[v - 1])) {
            *note = 0;
        }
        if ((note = &cells[i][c].notes[v - 1])) {
            *note = 0;
        }
    }
    /* Now do the same thing for the current box. */
    get_box(box, r, c);
    for (i = 0; i < ROW_LEN; i++) {
        if ((note = &box[i]->notes[v - 1])) {
            *note = 0;
        }
    }
}

/* Writes v as cell value and removes any lingering notes
 * on the same row, column or box. */
void solve_cell(int v, int r, int c) {
    write_cell(r, c, v + '0', false);
    remove_notes(v, r, c);
}


/* Main solver function.  */
void solve() {
    int r, c, v;
    for (r = 0; r < ROW_LEN; r++) {
        for (c = 0; c < ROW_LEN; c++) {
            note_possible_values(r, c);
            if ((v = last_remaining_cell(r, c))) {
                /* Can only be v for this cell, so we are done here! */
                solve_cell(v, r, c);
                continue;
            }
        }
    }
}
