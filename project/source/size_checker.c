#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

/* @date_added: 12/02/2025
 * @return_type: void
 * @params: window, and window height and width
 * @description: Checks window size limit
 */
void check_winsize(WINDOW *win,int height,int width){
    int temp_h=height,temp_w=width;
    getmaxyx(win,height,width);

    //Window size limit is set to 38X105 pixel
    if(height<38 || width<105 || height!=temp_h || width!=temp_w){
        endwin();
        printf("Program Crashed.\n");
        printf("Screen is maybe:\n   1. Too small\n   2. Changed\n");
        printf("Note: Do not resize window while program is running. Zoom out or resize terminal window beforehand for program to run properly.\n");
        exit(1);
    }
}

/* @date_added: 12/02/2025
 * @return_type: void
 * @params: window, string for status
 * @description: Makes a new window for about screen
 */
void status_bar(WINDOW *win,char *status){
    int height,width;
    getmaxyx(win,height,width);

    int len=strlen(status);

    wattrset(win,A_REVERSE);
    mvwprintw(win,1,(width-len)-2,"%s",status);
    wattrset(win,A_NORMAL);
}