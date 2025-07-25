#include "draw.h"  /* Needed for when we want to draw the solution. */
#include "grid.h"


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
    int count = 0, last_value, v;
    for (v = 1; v < 10; v++) {
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
    for (v = 1; v < 10; v++) {
        if (is_allowed(v, r, c)) {
            set_value(v, r, c, true);
        }
    }
}

/* Main solver function. */
void solve() {
    int r, c;
    for (r = 0; r < ROW_LEN; r++) {
        for (c = 0; c < ROW_LEN; c++) {
            note_possible_values(r, c);

        }
    }
}
