#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "rooms_struct.h"
#include "display.h"
#include "size_checker.h"

//README
// This file handles output on the terminal
// I think it would be best to put input handling on a separate
// C file
void Init()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    noqiflush();
    
    keypad(stdscr, TRUE);   
    ESCDELAY = 25;         
}

void SetWindows(WINDOW** menu_win, WINDOW** prompt_win)
{
	int maxy = getmaxy(stdscr);

	*menu_win = newwin(maxy-2, 0, 0, 0);
	*prompt_win = newwin(0, 0, maxy-1, 0);

    keypad(*prompt_win, TRUE);
}

void DisplayPrompt(WINDOW* prompt_win, char* prompt_str)
{
    wclear(prompt_win);

    wborder(prompt_win, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwprintw(prompt_win, 1, 2, "%s", prompt_str);

    wrefresh(prompt_win);
}

void DisplayList(WINDOW* menu_win, struct room* head, struct room* tail)
{
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx); 

    int list_h = maxy * 0.8;
    int list_w = maxx * 0.8;
    
    const int y_offset = -2;
    int start_y = (maxy - list_h) / 2 + y_offset; 
    int start_x = (maxx - list_w) / 2;

    WINDOW* list_win = newwin(list_h, list_w, start_y, start_x);
    if (list_win == NULL) {
        return; 
    }
    keypad(list_win, TRUE); 

    int total_rows = 0;
    for (struct room* t = head->nextnode; t != tail; t = t->nextnode)
        total_rows++;

    int scroll_offset = 0;

    // horizontal padding
    const int TABLE_WIDTH = 50; 
    int usable_width = list_w - 2; 
    int x_offset = ((usable_width - TABLE_WIDTH) / 2) + 1; 

    char *hline = "----------------------------------------------------";
    char *categories = "|     Room Name     | Starting | Current | Maximum |"; // Note: your string is 51 characters, this assumes 50 padding for centering

    while (1) {
        wclear(list_win);
        wborder(list_win, '|', '|', '-', '-', '+', '+', '+', '+');

        int max_data_y = list_h - 3;
        int available_lines = max_data_y - 4; 
        
        int visible_rows = available_lines / 2;
        if (visible_rows < 1) visible_rows = 1;
     
        int max_scroll_offset = total_rows - visible_rows;
        if (max_scroll_offset < 0) {
            max_scroll_offset = 0;
        }

        if (scroll_offset < 0) 
            scroll_offset = 0;

        if (scroll_offset > max_scroll_offset)
            scroll_offset = max_scroll_offset;

        int y = 2; // Start printing below the top border (y=0,1)

        // Print Header 
        mvwprintw(list_win, y, x_offset, hline);
        y++;
        mvwprintw(list_win, y, x_offset, categories);
        y++;

        struct room* temp = head->nextnode;

        // Skip rows based on scroll offset
        for (int i = 0; i < scroll_offset; i++)
            temp = temp->nextnode;

        for (int i = 0; i < visible_rows && temp != tail; i++, temp = temp->nextnode) {
            // Check to prevent writing outside the data area
            if (y + 1 >= max_data_y) break;
            
            mvwprintw(list_win, y, x_offset, hline);
            y++;
            
            // Print room data
            mvwprintw(list_win, y, x_offset,
                      "| %-17s | %-8d | %-7d | %-7d |",
                      temp->name, temp->pop_start,
                      temp->pop_current, temp->pop_max);
            y++;
        }

        mvwprintw(list_win, y, x_offset, hline);
        mvwprintw(list_win, list_h - 2, 2, "^/v scroll | Any other key = exit"); 

        wrefresh(list_win);

        int ch = wgetch(list_win); 

        if (ch == KEY_UP)
            scroll_offset--;
        else if (ch == KEY_DOWN)
            scroll_offset++;
        else
            break; 
    }

    delwin(list_win);
    touchwin(menu_win);
    wrefresh(menu_win);
}