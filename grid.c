#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "grid.h"

Cell cells[ROW_LEN][ROW_LEN] = {};
Cell answer[ROW_LEN][ROW_LEN] = {};

void read_grid(const char *fname, bool is_answer) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Could not open %s.\n", fname);
        exit(EXIT_FAILURE);
    }
    /* todo: make reading file safe */
    int r = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        for (int c = 0; c < ROW_LEN; c++) {
            Cell cell;
            cell.row = r;
            cell.col = c;
            cell.value = line[c] - '0';
            cell.is_constant = cell.value != 0;
            if (is_answer) {
                answer[r][c] = cell;
            } 
            else {
                cells[r][c] = cell;
            }
        }
        r++;
    }
    fclose(fp);
}


void print_grid(Cell grid[ROW_LEN][ROW_LEN]) {
    for (int r = 0; r < ROW_LEN; r++) {
        for (int c = 0; c < ROW_LEN; c++) {
            printf("%d", grid[r][c].value);
        }
        printf("\n");
    } 
}


/* Prints CELLS and ANSWER. */
void print_grids() {
    printf("Current grid:\n");
    print_grid(cells);
    printf("\nAnswer grid:\n");
    print_grid(answer);
}



void get_box(Cell box[BOX_LEN], int r, int c) {

}

bool is_complete(Cell cells[ROW_LEN][ROW_LEN], Cell answer[ROW_LEN][ROW_LEN]) {
    for (int i = 0; i < ROW_LEN; i++) {
        for (int j = 0; j < ROW_LEN; j++) {
            if (cells[i][j].value != answer[i][j].value) {
                return false;
            }
        }
    }
    return true;
}
