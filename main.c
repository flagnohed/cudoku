#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <strings.h>
#include "grid.h"

#define GAME_FILE "sudoku1.txt"
#define ANSWER_FILE "sudoku1-solved.txt"

extern Cell cells[ROW_LEN][ROW_LEN];
extern Cell answer[ROW_LEN][ROW_LEN];


int main() {
    /* Create two grids: one for the current state 
     * and one for the correct state. */

    read_grid (GAME_FILE, false);
    read_grid (ANSWER_FILE, true);
   
    print_grids();    
    printf("----------\n");    
    Cell subset[ROW_LEN] = {};
    
    get_row(subset, 1);
    print_subset(subset);
    
    get_col(subset, 1);
    print_subset(subset);

    get_box(subset, 1, 1);
    print_subset(subset);

    return 0;
}
