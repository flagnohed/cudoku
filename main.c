#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "draw.h"
#include "grid.h"
#include "solver.h"

#define GAME_DIR "sudokus/"
#define GAME_FILE GAME_DIR "sudoku1.txt"
#define ANSWER_FILE GAME_DIR "sudoku1-solved.txt"

Cell cells[ROW_LEN][ROW_LEN] = {};
Cell answer[ROW_LEN][ROW_LEN] = {};


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
    cells2screen(&y, &x);
    if (solver_mode) {
        solve();
    }

    do {
        draw_sudoku();
        move(y, x);
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
                write_cell(&y, &x, ch, note_mode);
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
    }   while (ch != 'q');

    endwin();
    return EXIT_SUCCESS;
}
