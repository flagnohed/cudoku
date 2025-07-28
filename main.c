#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "cell.h"
#include "solver.h"

/* Game files. */
#define GAME_DIR "sudokus/"
#define GAME_FILE GAME_DIR "sudoku1.txt"
#define ANSWER_FILE GAME_DIR "sudoku1-solved.txt"

/* Special keys. */
#define KEY_UARR 259
#define KEY_DARR 258
#define KEY_LARR 260
#define KEY_RARR 261
#define KEY_CTRLC 3

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
}   Direction_t;

Cell cells[ROW_LEN][ROW_LEN] = {};
Cell answer[ROW_LEN][ROW_LEN] = {};


/* Output all the notes for the current cell. */
void show_notes(int r, int c) {
    /* "Notes: " + "x, " * 9 + \0 = 7 + 27 + 1 = 35. */
    char note_str[35], tmp_str[4];
    int note_idx, cur_note;
    sprintf(note_str, "Notes: ");
    for (note_idx = 0; note_idx < ROW_LEN - 1; note_idx++) {
        cur_note = cells[r][c].notes[note_idx];
        if (cur_note) {
            sprintf(tmp_str, "%d, ", cur_note);
            strncat(note_str, tmp_str, 4);
        }
    }
    OUTPUT_MSG("%s", note_str);
}


void move_cursor(int *r, int *c, Direction_t dir) {
    switch (dir) {
        case DIR_UP:
            *r -= (*r > 0 ? 1 : 0);
            break;
        case DIR_LEFT:
            *c -= (*c > 0 ? 1 : 0);
            break;
        case DIR_DOWN:
            *r += (*r < ROW_LEN - 1 ? 1 : 0);
            break;
        case DIR_RIGHT:
            *c += (*c < ROW_LEN - 1 ? 1 : 0);
            break;
        default:
            /* Should never happen! */
            OUTPUT_MSG("move_cursor: unknown direction %d", dir);
            exit(EXIT_FAILURE);
    }
    show_notes(*r, *c);
}

/* Uses ncurses to draw CELLS. Using bold text to print
 * out the 9 in the grid (1 box = 3x3 cells). */
void draw_sudoku() {
    int x, y;
    char value;
    Cell *cell;
    move(Y, X);

    for (y = 0; y < ROW_LEN; y++) {
        if (y % BOX_LEN == 0) {
            attron(A_BOLD);
        }
        for (x = 0; x < ROW_LEN; x++) {
            if (y % BOX_LEN && x % BOX_LEN == 0) {
                /* This plus will indicate a corner of a box, so bold
                 * font should be applied even though y says we shouldn't. */
                attron(A_BOLD);
                printw("+");
                attroff(A_BOLD);
                printw("---");
                continue;
            }
            printw("+---");
        }
        /* Last plus will always indicate corner of a box, so it must be bold. */
        attron(A_BOLD);
        printw("+");
        attroff(A_BOLD);

        /* Top border done, now move down to values. */
        move(Y + 2 * y + 1, X);
        for (x = 0; x < ROW_LEN; x++) {
            cell = &cells[y][x];
            if (x % BOX_LEN == 0) {
                attron(A_BOLD);
            }
            printw("|");
            attroff(A_BOLD);
            value = (char) cell->value + '0';
            if (value == '0') {
                /* Don't print out 0 at the empty cells. */
                value = ' ';
            }
            if (cell->is_constant) {
                attron(A_BOLD);
            }
            printw(" %c ", value);
            attroff(A_BOLD);
        }
        attron(A_BOLD);
        printw("|");
        move(Y + 2 * y + 2, X);
        attroff(A_BOLD);
    }
    attron(A_BOLD);
    for (x = 0; x < ROW_LEN; x++)
        printw("+---");
    printw("+");
    attroff(A_BOLD);
    REFRESH_0();
}


/* Reads a grid from file FNAME. */
static void read_grid(const char *fname, bool is_answer) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int r = 0, c;
    fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Could not open %s.\n", fname);
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        for (c = 0; c < ROW_LEN; c++) {
            Cell cell;
            cell.value = line[c] - '0';
            cell.is_constant = cell.value != 0;
            if (is_answer)
                answer[r][c] = cell;
            else
                cells[r][c] = cell;
        }
        r++;
    }
    fclose(fp);
}


static void print_usage() {
    printf("Usage:\n");
    printf("./cudoku -[h|s] [FILE]\n");
    printf("    -h : prints this info and exits.\n");
    printf("    -s : solver mode. \n");
    printf("    FILE : sudoku game file. Should be placed in %s\n", GAME_DIR);
}


int main(int argc, char **argv) {
    int x = 0, y = 0, ch = 0;
    const char *fname;
    bool solver_mode = false, note_mode = false;
    switch (argc) {
        case 1:
            /* User did not give a sudoku file, so use the default one. */
            fname = &GAME_FILE[0];
            break;
        case 2:
            /* User either supplied file name or a flag. */
            if (strncmp(argv[1], "-h", 3) == 0) {
                print_usage();
                return EXIT_SUCCESS;
            }
            else if (strncmp(argv[1], "-s", 3) == 0) {
                solver_mode = true;
                fname = &GAME_FILE[0];
                break;
            }
            /* If it is not a valid flag, we assume user supplied
            * a file name. read_grid() will complain later otherwise,
            * so this assumption is OK to make. */
            fname = argv[1];
            break;
        case 3:
            if (strncmp(argv[1], "-s", 3) == 0) {
                solver_mode = true;
                fname = argv[2];
                break;
            }
            /* Intentional fall-through. */
        default:
            /* This will change later when we add difficulties. */
            print_usage();
            return EXIT_FAILURE;
    }

    /* Fill CELLS and ANSWER. */
    read_grid (fname, false);
    read_grid (ANSWER_FILE, true);

    initscr();             /* Initiate curses system. */
    raw();                 /* Line buffering disabled. */
    keypad(stdscr, TRUE);  /* F1, F2, arrow keys, ... */
    noecho();              /* Dont echo while we do getch. */

    if (solver_mode) {
        solve();
    }

    do {
        cells2screen(&y, &x);
        /* These functions need screen coordinates... */
        draw_sudoku();
        move(y, x);
        /* ...and the rest only cares about grid coordinates! */
        screen2cells(&y, &x);

        ch = tolower(getch());
        CLEAR_LINE(MSG_POS + 1);
        switch(ch) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                write_cell(y, x, ch, note_mode);
                break;
            case 'm':
                /* Toggle note mode. */
                note_mode = !note_mode;
                break;
            case KEY_UARR:
            case 'k':
                move_cursor(&y, &x, DIR_UP);
                break;
            case KEY_DARR:
            case 'j':
                move_cursor(&y, &x, DIR_DOWN);
                break;
            case KEY_LARR:
            case 'h':
                move_cursor(&y, &x, DIR_LEFT);
                break;
            case KEY_RARR:
            case 'l':
                move_cursor(&y, &x, DIR_RIGHT);
                break;
            case KEY_CTRLC:
                ch = 'q';
                break;
            default:
                break;
        }
    }   while (ch != 'q' && !is_complete());

    endwin();
    return EXIT_SUCCESS;
}
