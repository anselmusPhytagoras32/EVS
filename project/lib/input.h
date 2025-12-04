#ifndef INPUT_H
#define INPUT_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "display.h"

void prompt_input_str(WINDOW* prompt_win, char* buffer, int length);
int prompt_input_int(WINDOW* prompt_win, int length);
#endif
