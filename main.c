#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "grid.h"
#include "solver.h"

#define GAME_FILE "sudokus/sudoku1.txt"
#define ANSWER_FILE "sudokus/sudoku1-solved.txt"

extern Cell cells[ROW_LEN][ROW_LEN];
extern Cell answer[ROW_LEN][ROW_LEN];

void print_usage(int exit_code) {
    printf("Usage:\n");
    printf("./cudoku <file> (GAME_FILE if left blank)\n");
    exit(exit_code);
}


int main(int argc, char **argv) {
    int x, y, ch;
    const char *fname;
    bool solve_mode = false, note_mode = false;

    if (argc == 1) {
        /* User did not give a sudoku file, so use the default one. */
        fname = &GAME_FILE[0];
    }
    else if (argc == 2) {
        /* User either supplied file name or a flag. */
        if (strncmp(argv[1], "-h", 3) == 0) {
            print_usage(EXIT_SUCCESS);
        }
        else if (strncmp(argv[1], "-s", 3) == 0) {
            solve_mode = true;
        }
        /* If it is not a valid flag, we assume user supplied
         * a file name. read_grid() will complain later otherwise,
         * so this assumption is OK to make. */
        fname = argv[1];
    }
    else {
        /* This will change later when we add difficulties. */
        print_usage(EXIT_FAILURE);
    }
    /* Fill CELLS and ANSWER. */
    read_grid (fname, false);
    read_grid (ANSWER_FILE, true);
    initscr();             /* Initiate curses system. */
    raw();                 /* Line buffering disabled. */
    keypad(stdscr, TRUE);  /* F1, F2, arrow keys, ... */
    noecho();              /* Dont echo while we do getch. */

    x = 0;
    y = 0;
    ch = 0;

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
    return 0;
}
