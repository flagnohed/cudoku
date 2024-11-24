#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <strings.h>
#include "grid.h"
#include "draw.h"

#define GAME_FILE "sudokus/sudoku1.txt"
#define ANSWER_FILE "sudokus/sudoku1-solved.txt"

extern Cell cells[ROW_LEN][ROW_LEN];
extern Cell answer[ROW_LEN][ROW_LEN];

void print_usage() {
    printf("Usage:\n");
    printf("./cudoku <file> (GAME_FILE if left blank)");
}


int main(int argc, char **argv) {
    
    const char *fname;
    if (argc == 1) {
        /* User did not give a sudoku file, so use the default one. */
        fname = &GAME_FILE[0];
    }
    else if (argc == 2) {
        /* Assume this is an actual file name. read_grid() will 
         * complain if it's not. */
        fname = argv[1];
    }
    else {
        /* This will change later when we add difficulties. */
        print_usage();
        exit(EXIT_FAILURE);
    }

    /* Fill CELLS and ANSWER. */
    read_grid (fname, false);
    read_grid (ANSWER_FILE, true);
    initscr();             /* Initiate curses system. */
    raw();                 /* Line buffering disabled. */
    keypad(stdscr, TRUE);  /* F1, F2, arrow keys, ... */
    noecho();              /* Dont echo while we do getch. */

    /* Contains user pressed character. */
    int ch = 0;
    do {
        draw_sudoku(); 
        ch = tolower(getch());

    }   while (ch != 'q');

















    endwin();
    return 0;
}
