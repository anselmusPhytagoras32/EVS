#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "rooms_struct.h"
#include "rooms.h"
#include "input.h"
#include "display.h"

// Macro for amount of valid digits for pop_max
#define MAX_DIGIT 5
#define CHANGERATE 50

void createroom(struct room** head, struct room** tail, WINDOW* prompt_win) {
    struct room* temp = NULL;
    struct room* newroom = (struct room*)malloc(sizeof(struct room));
    if (newroom == NULL) return;
    memset(newroom, 0, sizeof(*newroom));

    DisplayPrompt(prompt_win, " Enter Room or Area Name:\t\t\t\t\t [Enter] Go back");
    prompt_input_str(prompt_win, newroom->name, sizeof(newroom->name));
    Up2low(newroom->name);

    if (strcmp(newroom->name, "") == 0) {
        free(newroom);
        werase(prompt_win);
        wrefresh(prompt_win);
        return;
    }

    if (strcmp(newroom->name, "outside") == 0) {
        DisplayPrompt(prompt_win, " The name \"outside\" is reserved. Choose another name.");
        free(newroom);
        return;
    }

    if (getroom(head, tail, &temp, newroom->name, prompt_win)) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Room with name \"%s\" already exists.", temp->name);
        DisplayPrompt(prompt_win, msg);
        free(newroom);
        return;
    }

    newroom->nextnode = *tail;
    newroom->prevnode = (*tail)->prevnode;
    newroom->prevnode->nextnode = newroom;
    (*tail)->prevnode = newroom;

    do {
        DisplayPrompt(prompt_win, " Enter maximum occupancy: ");
        newroom->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
        if (newroom->pop_max < 1) {
            DisplayPrompt(prompt_win, " Max occupancy must be greater than 0 (press any key to continue)");
	        wgetch(prompt_win);
	    }

    } while (newroom->pop_max < 1);
    
       if (newroom->pop_max == -1) { 
            free(newroom);
            werase(prompt_win);
            wrefresh(prompt_win);
            return;
        }

    do {
        DisplayPrompt(prompt_win, "Enter starting occupancy: ");
        newroom->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
        if(newroom->pop_start < 0){
            DisplayPrompt(prompt_win, "Start occupancy must be non-negative (press any key to continue)");
	        wgetch(prompt_win);
	    }

        while (newroom->pop_start > newroom->pop_max) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Starting occupancy %d cannot be greater than maximum occupancy %d (press any key to continue)",
            newroom->pop_start, newroom->pop_max);
            DisplayPrompt(prompt_win, msg);
	        wgetch(prompt_win);

            int choice = 0;
            do {
                DisplayPrompt(prompt_win, " Do you want to change max occupancy (1) or starting occupancy (2)? ");
                choice = wgetch(prompt_win);
                if (choice != '1' && choice != '2')
                    DisplayPrompt(prompt_win, " Invalid choice");
            } while (choice != '1' && choice != '2');

            switch (choice) {
                case '1':
                    DisplayPrompt(prompt_win, " Enter maximum occupancy: ");
                    newroom->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                    while (newroom->pop_max < 1) {
                        DisplayPrompt(prompt_win, " Max occupancy must be greater than 0 (press any key to continue)");
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter maximum occupancy: ");
                        newroom->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                    }
                    break;
                case '2':
                    DisplayPrompt(prompt_win, " Enter starting occupancy: ");
                    newroom->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                    while (newroom->pop_start < 0) {
                        DisplayPrompt(prompt_win, " Start occupancy must be non-negative (press any key to continue)");
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter starting occupancy: ");
                        newroom->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                    }
                    break;
            }
        }
    } while (newroom->pop_start < 0);

    newroom->pop_current = newroom->pop_start;
    newroom->pop_received = 0;
    newroom->pop_send_limit = newroom->pop_max * CHANGERATE / 100 + 1;

    if (newroom->pop_send_limit > newroom->pop_max)
        newroom->pop_send_limit = newroom->pop_max;

    char evac[32];
    do {
        DisplayPrompt(prompt_win, " Enter name of room to be evacuated to (outside is valid): ");
        prompt_input_str(prompt_win, evac, sizeof(evac));
        Up2low(evac);

        if (strcmp(evac, "outside") == 0) {
            temp = *tail;
            break;
        }
        if (!getroom(head, tail, &temp, evac, prompt_win))
            DisplayPrompt(prompt_win, " Room not found, enter a valid room name.");
    } while (!getroom(head, tail, &temp, evac, prompt_win) && strcmp(evac, "outside") != 0);

    newroom->evacroom = temp;

    werase(prompt_win);
    wrefresh(prompt_win);

    return ;
}


void deleteroom(struct room** head, struct room** tail, WINDOW *prompt_win) {
    struct room* temp = NULL;
    char name[32];
    DisplayPrompt(prompt_win, " Enter name of room to delete: ");
    prompt_input_str(prompt_win, name, sizeof(name));
    if (strcmp(name, "outside") == 0) {
        DisplayPrompt(prompt_win, " Cannot delete reserved room 'outside'. (press any key to continue)");
	    wgetch(prompt_win);
        return;
    }
    if (getroom(head, tail, &temp, name, prompt_win)) {
        char msg[128];
        snprintf(msg, sizeof(msg), " Room with name \"%s\" will be deleted (press any key to continue)", temp->name);
        DisplayPrompt(prompt_win,msg);
	    wgetch(prompt_win);
        temp->prevnode->nextnode = temp->nextnode;
        temp->nextnode->prevnode = temp->prevnode;
        free(temp);

        werase(prompt_win);
        wrefresh(prompt_win);

        return;
    }
    else {
        char msg[128];
        snprintf(msg, sizeof(msg), " Room with name \"%s\" could not be found (press any key to continue)", name);
        DisplayPrompt(prompt_win, msg);
	    wgetch(prompt_win);

        werase(prompt_win);
        wrefresh(prompt_win);

        return;
    }
}

void editroom(struct room** head, struct room** tail, WINDOW *prompt_win) {
    struct room* toedit = NULL;
    struct room* temp = NULL;
    char name[32];
    int choice = 0;
    DisplayPrompt(prompt_win, " Enter name of room to edit: ");
    prompt_input_str(prompt_win, name, sizeof(name));
    Up2low(name);

    if (getroom(head, tail, &toedit, name, prompt_win)) {
        char msg[128];
        snprintf(msg, sizeof(msg), " Room with name \"%s\" will be edited (press any key to continue)", toedit->name);
        DisplayPrompt(prompt_win, msg);
	    wgetch(prompt_win);

        while (choice != '5') {
            char msg[128];
            snprintf(msg, sizeof(msg), " What would you like to edit for room %s [1]name [2]max occupancy [3]starting occupancy [4]evac path [5]exit ", toedit->name);
            DisplayPrompt(prompt_win, msg);
            choice = wgetch(prompt_win);

            switch (choice) {
                case '1': {
                    DisplayPrompt(prompt_win, " Enter updated room name: ");
                    prompt_input_str(prompt_win, name, sizeof(name));
                    Up2low(name);

                    if (strcmp(name, "outside") == 0) {
                        DisplayPrompt(prompt_win, " The name 'outside' is reserved.");
                        break;
                    }
                    if (getroom(head, tail, &temp, name, prompt_win)) {
                        char msg[128];
                        snprintf(msg, sizeof(msg), " Room with name \"%s\" already exists ", temp->name);
                        DisplayPrompt(prompt_win, msg);
                    }
                    else {
                        strncpy(toedit->name, name, sizeof(toedit->name)-1);
                        toedit->name[sizeof(toedit->name)-1] = '\0';
                    }
                    break;
                }
                case '2': {
                    DisplayPrompt(prompt_win, " Enter updated room max occupancy: ");
                    toedit->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                    while (toedit->pop_max < 1) {
                        DisplayPrompt(prompt_win, " Max occupancy must be greater than 0 (press any key to continue)");
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter updated room max occupancy: ");
                        toedit->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                    }
                    while (toedit->pop_max < toedit->pop_start) {
                        char msg[128];
                        snprintf(msg, sizeof(msg), "Starting occupancy %d must not be greater than max occupancy %d (press any key to continue)", toedit->pop_start, toedit->pop_max);
                        DisplayPrompt(prompt_win, msg);
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter updated room max occupancy: ");
                        toedit->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                        while (toedit->pop_max < 1) {
                            DisplayPrompt(prompt_win, " Max occupancy must be greater than 0 (press any key to continue)");
			                wgetch(prompt_win);
                            DisplayPrompt(prompt_win, " Enter updated room max occupancy: ");
                            toedit->pop_max = prompt_input_int(prompt_win, MAX_DIGIT);
                        }
                    }
					toedit->pop_send_limit = toedit->pop_max * CHANGERATE / 100 + 1;
                    break;
                }
                case '3': {
                    DisplayPrompt(prompt_win, " Enter updated room starting occupancy: ");
                    toedit->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                    while (toedit->pop_start < 0) {
                        DisplayPrompt(prompt_win, " Start occupancy must be non-negative (press any key to continue)");
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter updated room starting occupancy: ");
                        toedit->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                    }
                    while (toedit->pop_start > toedit->pop_max) {
                        char msg[128];
                        snprintf(msg, sizeof(msg), "Starting occupancy %d must not be greater than max occupancy %d (press any key to continue)", toedit->pop_start, toedit->pop_max);
                        DisplayPrompt(prompt_win, msg);
			            wgetch(prompt_win);
                        DisplayPrompt(prompt_win, " Enter updated room starting occupancy: ");
                        toedit->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                        while (toedit->pop_start < 0) {
                            DisplayPrompt(prompt_win, " Start occupancy must be non-negative (press any key to continue)");
			                wgetch(prompt_win);
                            DisplayPrompt(prompt_win, " Enter updated room starting occupancy: ");
                            toedit->pop_start = prompt_input_int(prompt_win, MAX_DIGIT);
                        }
                    }
                    break;
                }
                case '4': {
                    char evac[32];
                    DisplayPrompt(prompt_win, " Enter name of room to be evacuated to(outside is a valid room): ");
                    prompt_input_str(prompt_win, evac, sizeof(evac));
                    Up2low(evac);

                    if (strcmp(evac, "outside") == 0) {
                        toedit->evacroom = *tail;
                        break;
                    }
                    while (!getroom(head, tail, &temp, evac, prompt_win)) {
                        char msg[128];
                        snprintf(msg, sizeof(msg), " Room with name \"%s\" not found, please enter a valid room(outside is a valid room):", evac);
                        DisplayPrompt(prompt_win, msg);
                        prompt_input_str(prompt_win, evac, sizeof(evac));
                        if (strcmp(evac, "outside") == 0) {
                            temp = *tail;
                            break;
                        }
                    }
                    toedit->evacroom = temp;
                    break;
                }
                case '5': return;
                default: DisplayPrompt(prompt_win, " Invalid choice, please try again");
                break;
            }
        }
    }
    else {
        char msg[128];
        snprintf(msg, sizeof(msg), " Room with name \"%s\" could not be found ", name);
        DisplayPrompt(prompt_win, msg);

        werase(prompt_win);
        wrefresh(prompt_win);

        return;
    }
}

int getroom(struct room** head, struct room** tail, struct room** temp, char name[], WINDOW *prompt_win) {
    struct room* current = (*head)->nextnode;
    while (current != NULL && current != *tail) {
        if (strcmp(current->name, name) == 0) {
            *temp = current;
            char msg[128];
            snprintf(msg, sizeof(msg), " getroom function Found room %s  ", current->name);
            DisplayPrompt(prompt_win, msg);
            return 1;
        }
        current = current->nextnode;
    }
    return 0;
}

/*void render(struct room* head, struct room* tail) {
    printf(" ------------------------------------------------  ");
    printf("|    Room Name    | Starting | Current | Maximum | ");
    for (struct room* temp = head->nextnode; temp != tail; temp = temp->nextnode) { //looks cursed but i think it's optimal
        printf(" ------------------------------------------------  ");
        printf("| %15s | %8d | %7d | %7d | ", temp->name, temp->pop_start, temp->pop_current, temp->pop_max);
    }
    printf(" ------------------------------------------------  ");
}*/

int countrooms(struct room* head, struct room* tail, WINDOW *prompt_win) {
    int count = 0;
    struct room* temp = head->nextnode;
    while (temp != tail) {
        temp = temp->nextnode;
        count++;
    }
    char msg[128];

    snprintf(msg, sizeof(msg), " countrooms function total rooms = %d ", count);
    DisplayPrompt(prompt_win, msg);

    werase(prompt_win);
    wrefresh(prompt_win);
    return count;
}


// Comparison function for qsort to sort rooms by distance (dist) in DESCENDING order.
int compare_rooms_desc(const void* a, const void* b) {
    const struct room* room_a = *(const struct room**)a;
    const struct room* room_b = *(const struct room**)b;
    
    // Sort in DESCENDING order (Farthest distance first).
    // If room_a is farther away (larger dist), it comes first (return -1).
    if (room_a->dist > room_b->dist) {
        return -1; 
    }
    // If room_b is farther away (larger dist), room_a is swapped down (return 1).
    if (room_a->dist < room_b->dist) {
        return 1;
    }
    
    return 0;
}


void sortrooms(struct room** head, struct room** tail, WINDOW *prompt_win) {
    // Dereference head/tail once for cleaner use
    struct room* sentinel_head = *head;
    struct room* sentinel_tail = *tail;

    int count = countrooms(sentinel_head, sentinel_tail, prompt_win);

    if (count <= 1) {
        DisplayPrompt(prompt_win, " sortrooms: 0 or 1 room, no sort needed ");
        return;
    }

    DisplayPrompt(prompt_win, " sortrooms: Getting room distances and preparing array ");
    
    struct room** room_array = (struct room**)malloc(count * sizeof(struct room*));
    if (room_array == NULL) {
        DisplayPrompt(prompt_win, " sortrooms: Memory allocation failed! ");
        return;
    }

    struct room* current = sentinel_head->nextnode;
    for (int i = 0; i < count; i++) {
        current->dist = getdistance(current, sentinel_tail, count, prompt_win);
        room_array[i] = current;
        current = current->nextnode;
    }
    
    DisplayPrompt(prompt_win, " sortrooms: Starting qsort() ");
    qsort(room_array, count, sizeof(struct room*), compare_rooms_desc);

    // Rebuild the Doubly Linked List from the Sorted Array
    DisplayPrompt(prompt_win, " sortrooms: Rebuilding linked list ");
    
    // Linking the stuffs up
    sentinel_head->nextnode = room_array[0];
    room_array[0]->prevnode = sentinel_head;

    for (int i = 0; i < count - 1; i++) {
        room_array[i]->nextnode = room_array[i+1];
        room_array[i+1]->prevnode = room_array[i];
    }

    room_array[count - 1]->nextnode = sentinel_tail;
    sentinel_tail->prevnode = room_array[count - 1];

    free(room_array);

    DisplayPrompt(prompt_win, " sortrooms sort finished ");
    werase(prompt_win);
    wrefresh(prompt_win);
}

int getdistance(struct room* tofind, struct room* tail, int count, WINDOW *prompt_win) {
    struct room* temp = tofind;
    for (int i = 1; i <= count; i++) {
        if (temp == NULL) return 0;
        temp = temp->evacroom;
        if (temp == tail) {
            char msg[128];
            snprintf(msg, sizeof(msg), " getdistance function distance for room %s is %d ", tofind->name, i);
            DisplayPrompt(prompt_win, msg);
            return i;
        }
        if (temp == NULL || temp->evacroom == NULL) {
            return 0;
        }
    }
    char msg[128];
    
    snprintf(msg, sizeof(msg), " getdistance function distance for room %s could not be found ", tofind->name);
    DisplayPrompt(prompt_win, msg);

    werase(prompt_win);
    wrefresh(prompt_win);

    return 0;
}

void getsavedata(struct room** head, struct room** tail, WINDOW *prompt_win) {
    FILE* ifp = fopen("save.txt", "r");
    if (ifp == NULL) {
        DisplayPrompt(prompt_win, "getsavedata No save data found");
        return;
    }
    else {
        /*
         * here we need to first delete all our existing nodes if we have any
         */
        DisplayPrompt(prompt_win, " getsavedata deleting existing nodes ");
        struct room* temp = (*head)->nextnode;
        while (temp != *tail && temp != NULL) {
            struct room* next = temp->nextnode;
            free(temp);
            temp = next;
        }
        (*head)->nextnode = *tail;
        (*tail)->prevnode = *head;

        int count;
        if (fscanf(ifp, "%d ", &count) != 1) {
            DisplayPrompt(prompt_win, " getsavedata: failed to read count ");
            fclose(ifp);
            return;
        }
        DisplayPrompt(prompt_win, " getsavedata Importing save data ");

        struct room* array[count + 2]; //+2 because head and tail are first and last nodes
        array[0] = *head;
        array[count + 1] = *tail;;
        DisplayPrompt(prompt_win, " getsavedata array created, creating nodes ");

        for (int i = 1; i <= count; i++) { //this is gonna look cursed
            array[i] = (struct room*)malloc(sizeof(struct room)); //make a struct for each pointer to point to
            memset(array[i], 0, sizeof(struct room));

            char line[256];
            if (fgets(line, sizeof(line), ifp) == NULL) {
                DisplayPrompt(prompt_win, " getsavedata: unexpected EOF ");
                for (int k = 1; k <= i; k++) if (array[k]) free(array[k]);
                fclose(ifp);
                return;
            }

            int evac_index = 0;
            int matched = sscanf(line, "%d, %d, %d, %d, %31s", &array[i]->pop_max, &array[i]->pop_start,
                    &array[i]->pop_send_limit, &evac_index, array[i]->name);

            if (matched < 5) {
                char msg[128];
                snprintf(msg, sizeof(msg), " getsavedata malformed input for room %d ", i);
                DisplayPrompt(prompt_win, msg);
                for (int k = 1; k <= i; k++) if (array[k]) free(array[k]);
                fclose(ifp);
                return;
            }

            // temporarily store evac_index in pop_received to preserve original struct usage
            array[i]->pop_received = evac_index;

            char msg[128];
            snprintf(msg, sizeof(msg), " getsavedata room %d data imported ", i);
            DisplayPrompt(prompt_win, msg);

            snprintf(msg, sizeof(msg), " pop max: %d pop start: %d pop send limit: %d evac path index: %d room name: %s ", array[i]->pop_max,
                    array[i]->pop_start, array[i]->pop_send_limit, array[i]->pop_received, array[i]->name);
            DisplayPrompt(prompt_win, msg);
            //the popreceived is fake. it's actually the index of the array that will be the evac path. this will be fixed later
        }
        DisplayPrompt(prompt_win, " getsavedata fixing node data ");
        for (int i = 1; i <= count; i++) { //fix node data that wasn't directly imported
            char msg[128];

            snprintf(msg, sizeof(msg), " getsavedata setting room data for room index %d ", i);
            DisplayPrompt(prompt_win, msg);
            array[i]->prevnode = array[i - 1];

            snprintf(msg, sizeof(msg), " getsavedata room %s prev node set to %s ", array[i]->name, array[i - 1]->name);
            DisplayPrompt(prompt_win, msg);
            array[i]->nextnode = array[i + 1];

            snprintf(msg, sizeof(msg), " getsavedata room %s next node set to %s ", array[i]->name, array[i + 1]->name);
            DisplayPrompt(prompt_win, msg);

            //this looks really cursed but this is the fix from 5 lines ago
            snprintf(msg, sizeof(msg), " getsavedata assigning evac room for room index %d ", i);
            DisplayPrompt(prompt_win, msg);

            // resolve evac index (treat 0 or out-of-range as tail/outside)
            int idx = array[i]->pop_received;
            if (idx <= 0 || idx > count + 1) {
                array[i]->evacroom = *tail;
            } else {
                array[i]->evacroom = array[idx];
            }

            snprintf(msg, sizeof(msg), " getsavedata evac room assigned for room index %d ", i);
            DisplayPrompt(prompt_win, msg);
            array[i]->pop_current = array[i]->pop_start; //technically the simulation will fix this when it's run anyways
            array[i]->pop_received = 0;
            array[i]->dist = 0; //this will get overwritten when the simulation is run anyways
        }
        DisplayPrompt(prompt_win, " getsavedata node data fixed, fixing head and tail ");
        (*head)->nextnode = array[1];
        (*tail)->prevnode = array[count];
    }
    DisplayPrompt(prompt_win, " getsavedata saved data imported.");

    werase(prompt_win);
    wrefresh(prompt_win);

    fclose(ifp);
}

void recordsavedata(struct room* head, struct room* tail, WINDOW *prompt_win) {
    FILE* ofp = fopen("save.txt", "w");

    if (ofp == NULL) {
        DisplayPrompt(prompt_win, " recordsavedata save file not found ");
        return;
    }
    else {
        struct room* temp = head;
        int count = countrooms(head, tail, prompt_win);

        DisplayPrompt(prompt_win, " recordsavedata creating array of rooms ");

        struct room* array[count + 2]; //this is cringe but it's the best way i can think to do it
        array[0] = head;
        for (int i = 1; i <= count + 1; i++) { //copy array pointers to point to each node of the linked list
            temp = temp->nextnode;
            array[i] = temp;
        }

        DisplayPrompt(prompt_win, " recordsavedata saving rooms to file ");

        fprintf(ofp, "%d\n", count);

        for (int i = 1; i <= count; i++) {
            int target = 0;
            for (int j = 0; j <= count + 1 && !target; j++) { //starts at 1 since head is a placeholder node
                if (array[i]->evacroom == array[j]) {
                    target = j;
                    //target will be the integer that corresponds to the index of the desired room to be set as the evacuation path in the getsavedata function
                }
            }
            fprintf(ofp, "%d, %d, %d, %d, %s\n", array[i]->pop_max, array[i]->pop_start, array[i]->pop_send_limit, target, array[i]->name);
        }
        DisplayPrompt(prompt_win, " recordsavedata rooms saved to file ");

        werase(prompt_win);
        wrefresh(prompt_win);
    }
    fclose(ofp);
}

void cleanrooms(struct room** head, struct room** tail, bool start, WINDOW *prompt_win) {
    struct room* temp = *head;
    while (temp != *tail) {
        temp = temp->nextnode;
        if (temp == NULL) break;
        if (start) {
            temp->pop_current = temp->pop_start;
        }
        temp->pop_received = 0;
    }
    DisplayPrompt(prompt_win, " cleanrooms rooms cleaned ");

    werase(prompt_win);
    wrefresh(prompt_win);
}

void Up2low(char *word){
    if(!word)
        return;
    
    for (int i = 0; i < strlen(word); i++) {
        word[i] = tolower((unsigned char)word[i]);
    }
}