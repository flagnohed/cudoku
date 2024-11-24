#ifndef GRID_H_
#define GRID_H_

#define BOX_LEN 3
#define ROW_LEN 9


typedef struct {
    int value;
    int row, col;
    bool is_constant;   /* True if cell not hidden at the start. */
}   Cell;

int get_closest_non_const(int r, int c);
int get_value(int r, int c);
bool is_allowed(int value, int r, int c);
bool is_constant(int r, int c);
void read_grid (const char *fname, bool is_answer);
void set_value(int val, int r, int c);
void print_grids();
void print_subset(Cell subset[ROW_LEN]);


#endif
