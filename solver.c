#include "cell.h"
#include "solver.h"

#define MAX_VAL 9


typedef enum {
    BOX,
    COLUMN,
    ROW,
}   Subset_t;

extern Cell cells[ROW_LEN][ROW_LEN];


/* This function should only be called if all cells have been correctly noted.
 * Returns the value if the given cell only has a single note, 0 otherwise. */
static int is_obvious_single(int r, int c) {
    int i, v, count = 0, last_value = 0;
    for (i = 0; i < ROW_LEN; i++) {
        /* Count the number of notes on this cell. */
        if ((v = cells[r][c].notes[i])) {
            /* Keep track of the last seen note,
             * as we want to return it if it is alone. */
            last_value = v;
            count++;
        }
    }
    return (count == 1 ? last_value : 0);
}


/* For the given coordinates, determine if there is only one possible value
 * that can go here. Returns that value if possible, else 0. */
static int last_remaining_cell(int r, int c) {
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
static void note_possible_values(int r, int c) {
    int v;
    for (v = 1; v <= MAX_VAL; v++) {
        if (cells[r][c].notes[v - 1] == 0 && is_allowed(v, r, c)) {
            set_value(v, r, c, true);
        }
    }
}


/* This function does not use get_{row, col, box}
 * because those helper functions give copies of that subset.
 * In this solver specific function we need to manipulate
 * these places in the actual cells grid. */
static void remove_notes(int v, int r, int c) {
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
static void solve_cell(int v, int r, int c) {
    write_cell(v, r, c, false);
    remove_notes(v, r, c);
}


/* Main solver function. Returns the number of solved cells. */
int solve(void) {
    int r, c, v = 0, num_solved_cells = 0;
    for (r = 0; r < ROW_LEN; r++) {
        for (c = 0; c < ROW_LEN; c++) {
            note_possible_values(r, c);
            if ((v = last_remaining_cell(r, c)) ||
                (v = is_obvious_single(r, c))) {
                /* Found the only possible value for this cell. Write it,
                 * remove any lingering notes and then call this function again
                 * to reiterate the entire grid to see if removing lingering notes
                 * revealed some other cell that can now be solved. */
                solve_cell(v, r, c);
                num_solved_cells += 1 + solve();
            }
        }
    }
    return num_solved_cells;
}
