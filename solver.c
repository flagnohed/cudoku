// #include "draw.h"  /* Needed for when we want to draw the solution. */
#include "grid.h"
// #include <stdio.h>
typedef enum {
    BOX,
    COLUMN,
    ROW,
}   Subset_t;

/* For a subset, check if we have one single empty cell. */
bool last_free_cell(Cell subset[ROW_LEN]) {
    int empty_cells = 0;
    for (int i = 0; i < ROW_LEN; i++) {
        if (subset[i].value == 0)
            empty_cells++;
    }
    return empty_cells == 1;
}

/* Last remaining cell helper function. */
void lrc_helper(Cell subset[ROW_LEN], Subset_t stype, int idx) {
    if (stype == ROW) {
        /* subset is already in the row-format, so
           just pass it along. */
        //return is_allowed()
    }

}

/* For a subset, check if we have exactly one possible
   location to place a certain value.
   Returns 0 if no value was found using this method,
   or the first value matching the requirements. */
int last_remaining_cell(Cell subset[ROW_LEN], Subset_t stype) {
    for (int i = 1; i < ROW_LEN + 1; i++) {
        /* Test all values 1-9. */
        for (int j = 0; j < ROW_LEN; j++) {

        }
    }
}
