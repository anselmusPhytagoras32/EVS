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

}

void SetWindows(WINDOW** menu_win, WINDOW** prompt_win)
{
	int maxy = getmaxy(stdscr);

	*menu_win = newwin(maxy-2, 0, 0, 0);
	*prompt_win = newwin(0, 0, maxy-1, 0);
}

void DisplayMenu(WINDOW* menu)
{

	wclear(menu);
	mvwprintw(menu, 1, 2, "1. insert a new room");
	mvwprintw(menu, 2, 2, "2. delete a room");
	mvwprintw(menu, 3, 2, "3. edit a room");
	mvwprintw(menu, 4, 2, "4. run simuation");
	mvwprintw(menu, 5, 2, "5. display list of rooms");
	mvwprintw(menu, 6, 2, "6. save current plan");
	mvwprintw(menu, 7, 2, "7. import saved plan");
	mvwprintw(menu, 8, 2, "0. exit");

	wrefresh(menu);

}

void DisplayPrompt(WINDOW* prompt_win, char* prompt_str)
{
    wclear(prompt_win);

    wborder(prompt_win, '|', '|', '-', '-', '+', '+', '+', '+');

    // Raise text higher and move it to row 1 (just below the top border)
    mvwprintw(prompt_win, 1, 2, "%s", prompt_str);

    wrefresh(prompt_win);
}

void DisplayList(WINDOW* menu_win, struct room* head, struct room* tail)
{
    // --- 1. Window Setup ---
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx); 

    int list_h = maxy * 0.8;
    int list_w = maxx * 0.8;
    int start_y = (maxy - list_h) / 2;
    int start_x = (maxx - list_w) / 2;

    WINDOW* list_win = newwin(list_h, list_w, start_y, start_x);

    if (list_win == NULL) {
        return; 
    }

    keypad(list_win, TRUE); 
    wclear(list_win);
    wborder(list_win, '|', '|', '-', '-', '+', '+', '+', '+');
    
    const int TABLE_WIDTH = 48; 
    
    int x_offset = (list_w - TABLE_WIDTH) / 2; 

    int y = 2;            
    int x = x_offset;       
    
    char buffer[17];
    char room_info[128];
    room_info[0] = '\0'; 
    
    char *vert_border = "----------------------------------------------------"; 
    char *categories = "|     Room Name     | Starting | Current | Maximum |";

    mvwprintw(list_win, y, x, vert_border);
    y += 1;
    mvwprintw(list_win, y, x, categories);
    y += 1;
    
    // Loop to print room data
    for(struct room* temp = head->nextnode; temp != tail; temp = temp->nextnode, y++)
    {
        // Check to prevent writing outside the window's vertical bounds
        if (y + 2 >= list_h) break; 
        
        mvwprintw(list_win, y, x, vert_border);
        y += 1;
        
        // Build the room data row string
        strcat(room_info, "| ");
        sprintf(buffer, "%-17s", temp->name); 
        strcat(room_info, buffer);
        strcpy(buffer, "");
        strcat(room_info, " | ");
        sprintf(buffer, "%-8d", temp->pop_start);
        strcat(room_info, buffer);
        strcpy(buffer, "");
        strcat(room_info, " | ");
        sprintf(buffer, "%-7d", temp->pop_current);
        strcat(room_info, buffer);
        strcpy(buffer, "");
        strcat(room_info, " | ");
        sprintf(buffer, "%-7d", temp->pop_max);
        strcat(room_info, buffer);
        strcpy(buffer, "");
        strcat(room_info, " |");

        mvwprintw(list_win, y, x, room_info);
        strcpy(room_info, "");
    }
    
    // Final horizontal border
    mvwprintw(list_win, y, x, vert_border);

    mvwprintw(list_win, list_h - 2, 2, "Press any key to return to the menu...");
    
    wrefresh(list_win); 
    
    flushinp(); // Clear the input buffer
    wgetch(list_win); 
    
    delwin(list_win); 

    touchwin(stdscr);       
    touchwin(menu_win);     
    wrefresh(menu_win);     
    refresh();              
}