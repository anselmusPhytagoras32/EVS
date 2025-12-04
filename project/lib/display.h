#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <locale.h>
#include "rooms_struct.h"

void Init();

void SetWindows(WINDOW** menu_win, WINDOW** prompt_win);

void DisplayMenu(WINDOW* menu);

void DisplayPrompt(WINDOW* prompt_win, char* prompt_str);

void DisplayList(WINDOW* menu_win, struct room* head, struct room* tail);
#endif
