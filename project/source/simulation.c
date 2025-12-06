#include <stdio.h>
#include <ncurses.h>
#include <string.h> // Ensure string.h is included for strcpy, strcat, etc.
#include "rooms_struct.h"
#include "simulation.h"
#include "rooms.h"
#include "input.h"
#include "display.h"

void runsimulation(WINDOW* menu_win, WINDOW *prompt_win, struct room** head, struct room** tail) {
    int ongoing = 1;
    int tosend;
    int sendlimit;
    int reclimit;
    
    // 1. SCROLLING VARIABLES
    int scroll_offset = 0;
    int total_rows = 0;
    for (struct room* t = (*head)->nextnode; t != *tail; t = t->nextnode) {
        total_rows++;
    }
    
    // AGGRESSIVE FIX: Ensure keypad is TRUE on the window receiving input
    keypad(menu_win, TRUE);

    sortrooms(head, tail, prompt_win);
    cleanrooms(head, tail, true, prompt_win);

    const char *header[] = {
        "     _            _      _            ",
        " ___(_)_ __ ___ _   _| | __ _| |_(_) ___ _ __ ",
        "/ __| | '_ ` _ \\| | | | |/ _` | __| |/ _ \\| '_ \\",
        "\\__ \\ | | | | | | |_| | | (_| | |_| | (_) | | | |",
        "|___/_|_| |_| |_|\\__,_|_|\\__,_|\\__|_|\\___/|_| |_|",
        "                                                 "
    };
    int header_rows = 6;
    int header_width = 49;

    DisplayPrompt(prompt_win, "runsimulation initialization done, starting simulation");

    do {
        // --- REDRAW STARTS HERE ---
        wclear(menu_win);
        wborder(menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
        int maxy, maxx;
        getmaxyx(menu_win, maxy, maxx);

        // Print header art
        for (int i = 0; i < header_rows; i++) {
            int tab = (maxx - header_width) / 2;
            mvwprintw(menu_win, i + 1, tab, "%s", header[i]);
        }
        
        // DYNAMIC ROW CALCULATION
        int y = header_rows + 4; // Starting Y position for the table
        int max_data_y = maxy - 3; 
        int available_lines = max_data_y - y; 
        int visible_rows = available_lines / 2;
        if (visible_rows < 1) visible_rows = 1;
        
        // SCROLL CLAMPING
        int max_scroll_offset = total_rows - visible_rows;
        if (max_scroll_offset < 0) max_scroll_offset = 0;
        if (scroll_offset < 0) scroll_offset = 0;
        if (scroll_offset > max_scroll_offset) scroll_offset = max_scroll_offset;

        const int TABLE_WIDTH = 52; 
        int x = (maxx - TABLE_WIDTH) / 2; 
        if (x < 1) x = 1; 

        char *vert_border = "----------------------------------------------------";
        char *categories = "|     Room Name     | Starting | Current | Maximum |";

        // Print Header
        mvwprintw(menu_win, y, x, vert_border);
        y += 1;
        mvwprintw(menu_win, y, x, categories);
        y += 1;
        
        // Print room data
        struct room* temp = (*head)->nextnode;
        
        // Skip rows based on scroll offset
        for (int i = 0; i < scroll_offset; i++) {
            if (temp == *tail) break;
            temp = temp->nextnode;
        }

        int printed_rows = 0;
        while(temp != *tail && printed_rows < visible_rows)
        {
            if (y + 1 >= max_data_y) break;
            
            mvwprintw(menu_win, y, x, vert_border);
            y += 1;
            
            mvwprintw(menu_win, y, x,
                      "| %-17s | %-8d | %-7d | %-7d |",
                      temp->name, temp->pop_start,
                      temp->pop_current, temp->pop_max);
            y += 1;

            temp = temp->nextnode;
            printed_rows++;
        }
        
        // DYNAMIC BOTTOM BORDER
        mvwprintw(menu_win, y, x, vert_border);
        
        // Prompt within the menu_win space
        mvwprintw(menu_win, maxy - 2, 2, "Use ^/v to scroll. [1]Continue | [0]Exit");
        wrefresh(menu_win); 

        for (struct room* sender = (*head)->nextnode; sender != *tail; sender = sender->nextnode) {
            if (sender->evacroom == NULL) {
                //just do nothing if room has nowhere to go
            }
            else {
                if (sender->pop_current - sender->pop_received < sender->pop_send_limit) {
                    sendlimit = sender->pop_current - sender->pop_received;
                }
                else {
                    sendlimit = sender->pop_send_limit;
                }

                if (sender->evacroom == *tail) {
                    sender->pop_current -= sendlimit;
                    sender->pop_received = 0;
                }
                else {
                    reclimit = sender->evacroom->pop_max - sender->evacroom->pop_current; 

                    if (sendlimit >= reclimit) { 
                        tosend = reclimit;
                    }
                    else {
                        tosend = sendlimit;
                    }

                    sender->pop_current -= tosend;
                    sender->pop_received = 0;
                    sender->evacroom->pop_current += tosend;
                    sender->evacroom->pop_received += tosend;
                }
            }
        }
        
        int response = 0;
        response = wgetch(menu_win); 
        
   
        if (response == KEY_UP || response == 72) {
            scroll_offset--;
        } 

        else if (response == KEY_DOWN || response == 80) {
            scroll_offset++;
        } 
        else if (response == '1') {
            ongoing = 1; 
        } else if (response == '0') {
            ongoing = 0; 
        } 
        
        werase(prompt_win);
        wrefresh(prompt_win);

    } while (ongoing);
}