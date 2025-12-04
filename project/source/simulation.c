#include <stdio.h>
#include <ncurses.h>
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
    sortrooms(head, tail, prompt_win);
    cleanrooms(head, tail, true, prompt_win);

    const char *header[] = {
        "     _                  _      _            ",
        " ___(_)_ __ ___  _   _| | __ _| |_(_) ___ _ __ ",
        "/ __| | '_ ` _ \\| | | | |/ _` | __| |/ _ \\| '_ \\",
        "\\__ \\ | | | | | | |_| | | (_| | |_| | (_) | | | |",
        "|___/_|_| |_| |_|\\__,_|_|\\__,_|\\__|_|\\___/|_| |_|",
        "                                                 "
    };
    int header_rows = 6;
    int header_width = 49;

    DisplayPrompt(prompt_win, "runsimulation initialization done, starting simulation");

    do {
        wclear(menu_win);
        wborder(menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
        int maxy, maxx;
        getmaxyx(menu_win, maxy, maxx);

        for (int i = 0; i < header_rows; i++) {
            int tab = (maxx - header_width) / 2;
            mvwprintw(menu_win, i + 1, tab, "%s", header[i]);
        }
        
        int y = header_rows + 4;
        const int TABLE_WIDTH = 48; 
        int x = (maxx - TABLE_WIDTH) / 2; 
   
        if (x < 1) x = 1; 

        char buffer[17];
        char room_info[128];
        room_info[0] = '\0'; 
        
        char *vert_border = "----------------------------------------------------"; 
        char *categories = "|     Room Name     | Starting | Current | Maximum |";

        mvwprintw(menu_win, y, x, vert_border);
        y += 1;
        mvwprintw(menu_win, y, x, categories);
        y += 1;
        
        // Loop to print room data
        for(struct room* temp = (*head)->nextnode; temp != *tail; temp = temp->nextnode, y++)
        {
            // Check to prevent writing outside the window's vertical bounds
            if (y + 2 >= maxy) break;
            
            mvwprintw(menu_win, y, x, vert_border);
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

            mvwprintw(menu_win, y, x, room_info);
            strcpy(room_info, "");
        }
        
        mvwprintw(menu_win, y, x, vert_border);
        
        // The original simulation logic continues here...
        for (struct room* sender = (*head)->nextnode; sender != *tail; sender = sender->nextnode) {
            // ... (simulation logic remains unchanged) ...
            if (sender->evacroom == NULL) {
                //just do nothing if room has nowhere to go
            }
            else {
                if (sender->pop_current - sender->pop_received < sender->pop_send_limit) {
                    sendlimit = sender->pop_current - sender->pop_received;
                    /*
                     *amount we want to send is equal to the smaller number between population left from last cycle,
                     *and limit of population that can be moved in one iteration
                     */
                }
                else {
                    sendlimit = sender->pop_send_limit;
                }

                if (sender->evacroom == *tail) { //if we are going outside, just send it. no limits to what outside can accept
                    sender->pop_current -= sendlimit;
                    sender->pop_received = 0;
                }

                else { //if we go to another room, then we need to calculate how much that room can accept
                    reclimit = sender->evacroom->pop_max - sender->evacroom->pop_current; //amount of people room can still hold

                    if (sendlimit >= reclimit) { //just pick amount to send based on lesser of the two limits
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
        
        mvwprintw(prompt_win, 2, 2, "Continue? [1]Yes / [0]No");
        wrefresh(menu_win); 

        // Lazy attempt to polish input taking --miko
        int response = 0;
        while(response != '1' && response != '0')
        {
            response = wgetch(prompt_win);
            switch(response)
            {
                case '1':
                ongoing = 1;
                break;
                case '0':
                ongoing = 0;
                werase(prompt_win);
                wrefresh(prompt_win);
                break;
            }
        }

        response = 0;
        // resets response to default state --miko
    }
    while (ongoing);
}
