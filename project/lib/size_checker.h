#ifndef GLOBAL_H
#define GLOBAL_H

#include <ncurses.h>

//Global functions
void check_winsize(WINDOW *win,int height,int width);
void status_bar(WINDOW *win,char *status);

#endif
