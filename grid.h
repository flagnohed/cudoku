#ifndef GRID_H_
#define GRID_H_

#define BOX_LEN 3
#define ROW_LEN 9

typedef struct {
    int value;
    int row, col;
    bool is_constant;   /* True if cell not hidden at the start. */
}   Cell;

void print_grids();
void read_grid (const char *fname, bool is_answer);

#endif
