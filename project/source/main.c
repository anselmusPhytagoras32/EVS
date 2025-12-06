#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "rooms_struct.h"
#include "rooms.h"
#include "simulation.h"
#include "display.h"
#include "size_checker.h"

void draw_title_menu(WINDOW *menu_win, int width, int height) {
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
    int title_rows = sizeof(title)/sizeof(title[0]);

    const char *menu[] = {
        "[1] insert a new room",
        "[2] delete a room",
        "[3] edit a room",
        "[4] run simulation",
        "[5] display list of rooms",
        "[6] save current plan",
        "[7] import saved plan",
        "[0] exit"
    };
    int menu_rows = sizeof(menu)/sizeof(menu[0]);

    wclear(menu_win);
    wborder(menu_win,'|','|','-','-','+','+','+','+');

    // Draw title centered
    for(int i=0;i<title_rows;i++){
        if(i==0) wattrset(menu_win,A_BOLD);
        else if(i==9) wattrset(menu_win,A_ITALIC);

        int len = strlen(title[i]);
        int x = (width - len) / 2;
        int y = i + 3; // top padding
        mvwprintw(menu_win, y, x, "%s", title[i]);
    }
    
    int start_y = title_rows + 8;
    // Compute left margin based on longest menu string
    int max_len = 0;
    for(int i=0;i<menu_rows;i++){
        int len = strlen(menu[i]);
        if(len > max_len) max_len = len;
    }
    int start_x = (width - max_len) / 2;

    // Draw menu left-aligned but centered block
    for(int i=0;i<menu_rows;i++){
        int y = start_y + i*2;
        mvwprintw(menu_win, y, start_x, "%s", menu[i]);
    }
    
    wattrset(menu_win,A_NORMAL);
    status_bar(menu_win,"Menu");
    wrefresh(menu_win);
}

int main(void) {
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

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);
    int window_width = width;

    WINDOW *menu_win = newwin(height, window_width, 0, 0);
    WINDOW *prompt_win = newwin(6, window_width, height-6, 0);

    scrollok(stdscr, FALSE);
    scrollok(menu_win, FALSE);
    scrollok(prompt_win, FALSE);
    idlok(menu_win, FALSE);
    idlok(prompt_win, FALSE);

    mousemask(0, NULL);
    mouseinterval(0);

    check_winsize(menu_win, height, window_width);

    if(!menu_win){
        printf("Failed to load screen\n");
        exit(1);
    }

    while(choice != '0') {
        int old_h = height, old_w = width;
        getmaxyx(stdscr, height, width);
        if(height != old_h || width != old_w){
            delwin(menu_win);
            delwin(prompt_win);

            menu_win = newwin(height, width, 0, 0);
            prompt_win = newwin(6, width, height-6, 0);

            check_winsize(menu_win, height, width);
        }

        draw_title_menu(menu_win, width, height);
        choice = wgetch(menu_win);

        switch(choice){
            case '1': createroom(&head,&tail,prompt_win); break;
            case '2': deleteroom(&head,&tail,prompt_win); break;
            case '3': editroom(&head,&tail,prompt_win); break;
            case '4': runsimulation(menu_win,prompt_win,&head,&tail); break;
            case '5': DisplayList(menu_win, head, tail); break;
            case '6': recordsavedata(head, tail, prompt_win); break;
            case '7': getsavedata(&head, &tail, prompt_win); break;
            case '0':
                delwin(menu_win);
                delwin(prompt_win);
                endwin();
                return 0;
        }
    }
}
