// #include "draw.h"  /* Needed for when we want to draw the solution. */
#include "grid.h"
// #include <stdio.h>


typedef enum {
    BOX,
    COLUMN,
    ROW,
}   Subset_t;


/* For a subset, check if we have one single empty cell.
 * Returns 0 if no last free cell found, or value between 1-9
 * if found. */
int last_free_cell(Cell subset[ROW_LEN]) {
    int i, found_value;
    int unseen[ROW_LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int empty_cells = 0;

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

