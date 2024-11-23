#ifndef GRID_H_
#define GRID_H_

#define BOX_LEN 3
#define ROW_LEN 9

typedef struct {
    int value;
    int row, col;
    bool is_constant;   /* True if cell not hidden at the start. */
}   Cell;


void get_row(Cell row[ROW_LEN], int r);
void get_col(Cell col[ROW_LEN], int c);
void get_box(Cell box[ROW_LEN], int r, int c);

void print_grids();
void read_grid (const char *fname, bool is_answer);

void print_subset(Cell subset[ROW_LEN]);
#endif
