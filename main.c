#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "cell.h"
#include "solver.h"

/* Game files. */
#define GAME_DIR "sudokus/"
#define GAME_FILE GAME_DIR "sudoku1.txt"
#define ANSWER_FILE GAME_DIR "sudoku1-solved.txt"
#define SUDOKUS_EASY   GAME_DIR "easy.txt"
#define SUDOKUS_MEDIUM GAME_DIR "medium.txt"
#define SUDOKUS_HARD   GAME_DIR "hard.txt"

/* Special keys. */
#define KEY_UARR 259
#define KEY_DARR 258
#define KEY_LARR 260
#define KEY_RARR 261
#define KEY_CTRLC 3

/* Number of sudokus in each difficulty
 * (sudokus/{difficulty}.txt) */
#define NUM_SUDOKUS_EASY   100000
#define NUM_SUDOKUS_MEDIUM 352643
#define NUM_SUDOKUS_HARD   321592

#define BUF_SIZE 256
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
}   Direction_t;

Cell cells[ROW_LEN][ROW_LEN] = {0};
Cell answer[ROW_LEN][ROW_LEN] = {0};


/* Output all the notes for the current cell. */
static void show_notes(int r, int c) {
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


static void move_cursor(int *r, int *c, Direction_t dir) {
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
static void draw_sudoku(void) {
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
static void read_grid(const char *fname, const int sudoku_number) {
    FILE *fp;
    char *line = NULL, linebuf[BUF_SIZE];
    int v = 0, r = 0, c = 0, row = 1;

    fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Could not open %s.\n", fname);
        exit(EXIT_FAILURE);
    }
    while (fgets(linebuf, BUF_SIZE, fp) != NULL) {
        /* Read until we find the correct line in fp. */
        if (row == sudoku_number) { break; }
        row++;
    }
    /* Found the row containing the sudoku grid!
     * They are partitioned like this: [HASH] [GRID] [DIFFICULTY],
     * which means we can skip until after the first whitespace
     * and parse until the next whitespace after that. */
    line = &linebuf[0];
    while (*line != ' ') {
        line++;
    }
    /* Also skip the actual whitespace. After that we can start
     * parsing the sudoku grid. */
    line++;
    while (*line != ' ') {
        // values[count] = *line - '0';
        // printf("%d %d \n", count, r);
        Cell cell;
        v = *line - '0';
        cell.value = v;
        cell.is_constant = (bool) v;
        cells[r][c] = cell;
        /* @todo: figure out why modulo doesn't work here. */
        if (++c == ROW_LEN) {
            c = 0;
            r++;
        }
        line++;
    }
    fclose(fp);
}


static void print_usage(const char *context) {
    printf("%s\n", context);
    printf("./cudoku [options] [FILE] \n");
    printf("[options]: \n");
    printf("-h, --help\n\tprints this info and exits. \n");
    printf("-s, --solve\n\tsolver mode. \n");
    printf("-d, --difficulty [easy|medium|hard]\n\tsets difficulty. Default: easy. \n");
    printf("-n, --number [n|r]\n\tchooses sudoku number n from the chosen difficulty, \n");
    printf("\tor a random one if r is supplied. Default: 1. \n");
}


int main(int argc, char **argv) {
    int i, r = 0, c = 0, ch = 0, sudoku_number = 1;
    bool solver_mode = false, note_mode = false;
    const char *fname = SUDOKUS_EASY;

    for (i = 1; i < argc; i++) {
        /* strcmp is OK here since we compare command line arguments
         * (which are guaranteed to be null-terminated) and literals
         * (which also are guaranteed to be null-terminated). */
        if (strcmp(argv[i], "-h") == 0 ||
                strcmp(argv[i], "--help") == 0) {
            print_usage("Usage: ");
            exit(EXIT_SUCCESS);
        }
        if (strcmp(argv[i], "-s") == 0 ||
                strcmp(argv[i], "--solve") == 0) {
            solver_mode = true;
            continue;
        }
        if (strcmp(argv[i], "-d") == 0 ||
                strcmp(argv[i], "--difficulty") == 0) {
            if (i + 1 == c) {
                print_usage("Found difficulty flag without difficulty.");
                exit(EXIT_FAILURE);
            }
            i++;
            if (strcmp(argv[i], "easy") == 0) {
                fname = SUDOKUS_EASY;
            }
            else if (strcmp(argv[i], "medium") == 0) {
                fname = SUDOKUS_MEDIUM;
            }
            else if (strcmp(argv[i], "hard") == 0) {
                fname = SUDOKUS_HARD;
            }
            else {
                print_usage("Unknown difficulty");
                exit(EXIT_FAILURE);
            }
        }
        if (strcmp(argv[i], "-n") == 0 ||
                strcmp(argv[i], "--number") == 0) {
            if (i + 1 == c) {
                print_usage("Found number flag without number.");
                exit(EXIT_FAILURE);
            }
            if (sscanf(argv[++i], "%d", &sudoku_number) != 1) {
                print_usage("Failed to parse sudoku number.");
                exit(EXIT_FAILURE);
            }
        }
    }
    read_grid (fname, sudoku_number);
    if (solver_mode) {
        solve();
        if (is_complete()) {
            endwin();
            printf("Sudoku %d from %s solved! \n",
                    sudoku_number, fname);
            return EXIT_SUCCESS;
        }
    }
    initscr();             /* Initiate curses system. */
    raw();                 /* Line buffering disabled. */
    keypad(stdscr, TRUE);  /* F1, F2, arrow keys, ... */
    noecho();              /* Dont echo while we do getch. */
    do {
        cells2screen(&r, &c);
        /* These functions need screen coordinates... */
        draw_sudoku();
        move(r, c);
        /* ...and the rest only cares about grid coordinates! */
        screen2cells(&r, &c);
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
                write_cell(ch - '0', r, c, note_mode);
                if (is_complete() && !note_mode) {
                    ch = 'q';
                }
                break;
            case 'm':
                /* Toggle note mode. */
                note_mode = !note_mode;
                break;
            case KEY_UARR:
            case 'k':
                move_cursor(&r, &c, DIR_UP);
                break;
            case KEY_DARR:
            case 'j':
                move_cursor(&r, &c, DIR_DOWN);
                break;
            case KEY_LARR:
            case 'h':
                move_cursor(&r, &c, DIR_LEFT);
                break;
            case KEY_RARR:
            case 'l':
                move_cursor(&r, &c, DIR_RIGHT);
                break;
            case KEY_CTRLC:
                ch = 'q';
                break;
            default:
                break;
        }
    }   while (ch != 'q');
    endwin();
    return EXIT_SUCCESS;
}
