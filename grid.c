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


/* Prints a row, column or box. */
void print_subset(Cell subset[ROW_LEN]) {
    for (int i = 0; i < ROW_LEN; i++) {
        printf("%d", subset[i].value);
    }
    printf("\n");
}

void print_grid(Cell grid[ROW_LEN][ROW_LEN]) {
    for (int r = 0; r < ROW_LEN; r++) {
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
 * This is because it will be easier to traverse it. */
void get_box(Cell box[ROW_LEN], int r, int c) {
    /* Figure out where the start of the current box is. */
    int start_r = r - r % 3;
    int start_c = c - c % 3;
    int count = 0;
    
    for (int i = start_r; i < start_r + 3; i++) {
        for (int j = start_c; j < start_c + 3; j++) {
            box[count] = cells[i][j];
            count++;
        }
    }
}


/* Gets row R from cells and puts it in ROW. */
void get_row(Cell row[ROW_LEN], int r) {
    for (int i = 0; i < ROW_LEN; i++) {
        row[i] = cells[r][i];
    }
}


/* Gets column C from cells and puts it in col. */
void get_col(Cell col[ROW_LEN], int c) {
    for (int i = 0; i < ROW_LEN; i++) {
        col[i] = cells[i][c];
    }
}


bool is_complete() {
    for (int i = 0; i < ROW_LEN; i++) {
        for (int j = 0; j < ROW_LEN; j++) {
            if (cells[i][j].value != answer[i][j].value) {
                return false;
            }
        }
    }
    return true;
}
