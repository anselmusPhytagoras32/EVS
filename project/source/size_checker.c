#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MIN_HEIGHT 38
#define MIN_WIDTH 105

/* @description: Checks window size and auto-resizes windows if possible */
void check_winsize(WINDOW *win, int height, int width){
    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);

    if(term_h < MIN_HEIGHT || term_w < MIN_WIDTH){
        // Terminal too small, show message and wait
        endwin();
        printf("Terminal is too small.\n");
        printf("Minimum size: %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
        printf("Current size: %dx%d\n", term_w, term_h);
        printf("Please resize your terminal and run again.\n");
        exit(1);
    }

    // Resize the ncurses window to current terminal size
    wresize(win, term_h, term_w);
    mvwin(win, 0, 0);
    wclear(win);
    box(win, 0, 0);
    wrefresh(win);
}

void status_bar(WINDOW *win, char *status){
    int height,width;
    getmaxyx(win,height,width);

    int len = strlen(status);

    wattrset(win,A_REVERSE);
    mvwprintw(win,1,(width-len)-2,"%s",status);
    wattrset(win,A_NORMAL);
}
