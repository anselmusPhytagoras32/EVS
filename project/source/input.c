#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

#include <stdio.h>
#include <ncurses.h>
#include <ctype.h> // For isdigit()

// The DisplayPrompt function is assumed to be defined elsewhere and 
// takes a WINDOW* and a message string.

// Helper function to create and position the sub-window
WINDOW* create_input_subwin(WINDOW* parent_win, int sub_h, int* sub_w_out, int* starty_out, int* startx_out) {
    int parent_h, parent_w, parent_y, parent_x;
    getmaxyx(parent_win, parent_h, parent_w);
    getbegyx(parent_win, parent_y, parent_x);

    *sub_w_out = parent_w - 2;
    // Position the sub-window just above the bottom border of the parent_win
    *starty_out = parent_y + parent_h - sub_h - 1;
    *startx_out = parent_x + 1;

    WINDOW* sub_win = subwin(parent_win, sub_h, *sub_w_out, *starty_out, *startx_out);
    if (sub_win) {
        box(sub_win, 0, 0);
        wrefresh(sub_win);
    }
    return sub_win;
}

// Function to handle string input (simplified for Ctrl+X demo)
void prompt_input_str(WINDOW* parent_win, char* buffer, int length) {
    int sub_w, starty, startx;
    int sub_h = 3;

    WINDOW* sub_win = create_input_subwin(parent_win, sub_h, &sub_w, &starty, &startx);
    if (!sub_win) return;
    
    // Set cursor position inside the box
    wmove(sub_win, 1, 1);
    
    // Use mvwgetnstr for simplicity here, as string input often needs to be non-blocking later.
    // For full Ctrl+X functionality, this would also need a character-by-character loop.
    echo();
    mvwgetnstr(sub_win, 1, 1, buffer, length - 1);
    noecho();

    delwin(sub_win);
    // Refresh parent_win to remove sub_win remnants (or the caller handles it)
    wrefresh(parent_win); 
}

// Function to handle integer input with Ctrl+X exit
int prompt_input_int(WINDOW* parent_win, int length) {
    int sub_w, starty, startx;
    int sub_h = 3;
    int ch;
    int i = 0;
    
    // Allocate buffer for input, plus space for null terminator
    char buffer[length + 1]; 
    memset(buffer, 0, sizeof(buffer));

    WINDOW* sub_win = create_input_subwin(parent_win, sub_h, &sub_w, &starty, &startx);
    if (!sub_win) return -1;
    
    // Turn on keypad and noecho for manual input handling
    keypad(sub_win, TRUE);
    noecho();
    
    wmove(sub_win, 1, 1); // Move cursor to input spot
    wrefresh(sub_win);

    while ( (ch = wgetch(sub_win)) != '\n' ) {
        int y, x;
        getyx(sub_win, y, x);

        if (ch == 24) { // ASCII for Ctrl+X
            delwin(sub_win);
            wrefresh(parent_win);
            return -1; // Return -1 to signal exit
        } 
        
        else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                mvwprintw(sub_win, y, x - 1, " ");
                wmove(sub_win, y, x - 1);
                wrefresh(sub_win);
            }
        } 
        
        else if (isdigit(ch) && i < length) { // Digits 0-9
            buffer[i] = ch;
            mvwprintw(sub_win, y, x, "%c", ch);
            i++;
            wrefresh(sub_win);
        }
        // Ignore all other keys
    }

    // Input loop finished (user pressed Enter)
    delwin(sub_win);
    wrefresh(parent_win); // Refresh parent to clean up sub_win remnants

    if (i == 0) {
        return 0; // Treat empty input as 0 or handle as error
    }

    // Convert input string to integer
    buffer[i] = '\0';
    char* endptr;
    long result_long = strtol(buffer, &endptr, 10);
    int result = (int)result_long;

    // Check for invalid characters (shouldn't happen with the manual loop) or overflow
    if (*endptr != '\0' || result_long != result) {
        // Use the PARENT window to display the error, as the sub_win is deleted
        DisplayPrompt(parent_win, "error: input contains invalid characters or is too large.");
        return -1;
    }

    return result;
}
