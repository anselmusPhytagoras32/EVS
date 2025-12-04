//Authors:
//Aaron Alexander Beard
//Manuel Miko Delola
//Gebhel Anselm Santos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "rooms_struct.h"
#include "rooms.h"
#include "simulation.h"
#include "display.h"
#include "size_checker.h"

int main(void) {
    /*
     * head and tail are reference nodes, NOT data containing nodes for rooms.
     * an empty list will always contain the head and tail reference nodes
     * tail will also function as a placeholder node for "outside"
     */
    struct room* head = malloc(sizeof(struct room));
    struct room* tail = malloc(sizeof(struct room));

    head->nextnode = tail;
    head->prevnode = NULL;
    tail->nextnode = NULL;
    tail->prevnode = head;

    strcpy(head->name, "head");
    strcpy(tail->name, "outside");
    head->evacroom = NULL;
    tail->evacroom = NULL;
    head->pop_max = CHANGERATE;

    Init();
    int choice = -1;

    //get terminal size
      int height,width;
	getmaxyx(stdscr,height,width);

	int window_width=width/2;

	WINDOW *menu_win=newwin(height,window_width,0,width/4);
    WINDOW *prompt_win = newwin(6, window_width, height-6, width/4);

	check_winsize(menu_win,height,window_width);

	if(!menu_win){
		printf("Failed to load screen\n");
		exit(1);
	}

	// indentations are probably borked because I just cut
	// and pasted this line this in vim
    while (choice != '0') {

	wclear(menu_win);
	wborder(menu_win,'|','|','-','-','+','+','+','+');
      wrefresh(menu_win); 

      const char *title[] = {
            "8 8888888888 `8.`888b           ,8' d888888o.   ",
            "8 8888        `8.`888b         ,8'.`8888:' `88. ",
            "8 8888         `8.`888b       ,8' 8.`8888.   Y8 ",
            "8 8888          `8.`888b     ,8'  `8.`8888.     ",
            "8 888888888888   `8.`888b   ,8'    `8.`8888.    ",
            "8 8888            `8.`888b ,8'      `8.`8888.   ",
            "8 8888             `8.`888b8'        `8.`8888.  ",
            "8 8888              `8.`888'     8b   `8.`8888. ",
            "8 8888               `8.`8'      `8b.  ;8.`8888 ",
            "8 888888888888        `8.`        `Y8888P ,88P' ",
			"",
			"A Dynamic Evacuation Planning and Simulation Tool"
      };

      
	//Print line by line of constant title
	int title_row_size=sizeof(title)/sizeof(title[0]);
	for(int i=0;i<title_row_size;i++){
		if(i==0){
			wattrset(menu_win,A_BOLD);
		}
		else if(i==9){
			wattrset(menu_win,A_ITALIC);
		}

		int len=strlen(title[i]);
		int tab=(window_width-len)/2;
		mvwprintw(menu_win,i+5,tab,"%s",title[i]);
	}
	wattrset(menu_win,A_NORMAL);

	const char *menu[]={
		"[1] insert a new room",
		"[2] delete a room",
		"[3] edit a room",
		"[4] run simuation",
        "[5] display list of rooms",
        "[6] save current plan",
        "[7] import saved plan",
        "[0] exit"
	};

      wrefresh(menu_win); 

	//Print line by line of constant menu
	int menu_row_size=sizeof(menu)/sizeof(menu[0]);
	for(int i=0;i<menu_row_size;i++){
		int len=strlen(menu[i]);
		mvwprintw(menu_win,(height/2)+i*2,(window_width-len)/2,"%s",menu[i]);
	}

	status_bar(menu_win,"Menu");

	choice = wgetch(menu_win);

        switch(choice) {
            case '1': 
		createroom(&head, &tail, prompt_win);
		break;
            case '2': 
		deleteroom(&head, &tail, prompt_win); 
		break;
            case '3':
		editroom(&head, &tail, prompt_win);
		break;
            case '4':
		runsimulation(menu_win, prompt_win, &head, &tail);
		break;
            case '5':
			DisplayList(menu_win, head, tail);
            wclear(menu_win);
            wborder(menu_win,'|','|','-','-','+','+','+','+');

            //Redraw the title 
            int title_row_size=sizeof(title)/sizeof(title[0]);
            for(int i=0;i<title_row_size;i++){
                if(i==0){ wattrset(menu_win,A_BOLD); }
                else if(i==9){ wattrset(menu_win,A_ITALIC); }

                int len=strlen(title[i]);
                int tab=(window_width-len)/2;
                mvwprintw(menu_win,i+5,tab,"%s",title[i]);
            }
            wattrset(menu_win,A_NORMAL);
            
            // Redraw the menu options 
            int menu_row_size=sizeof(menu)/sizeof(menu[0]);
            for(int i=0;i<menu_row_size;i++){
                int len=strlen(menu[i]);
                mvwprintw(menu_win,(height/2)+i*2,(window_width-len)/2,"%s",menu[i]);
            }

            //Redraw the status bar
            status_bar(menu_win,"Menu");
            wrefresh(menu_win);

			mvwprintw(prompt_win, 2, 2, "press any key to continue");
            wgetch(prompt_win);
		break;
            case '6':
			recordsavedata(head, tail, prompt_win); 
		break;
            case '7':
			getsavedata(&head, &tail, prompt_win); 
		break;
            case '0':
			delwin(menu_win);
			delwin(prompt_win);
			endwin();
			return 0;
        }
      wborder(menu_win,'|','|','-','-','+','+','+','+');
      wrefresh(menu_win); 
    }
}
