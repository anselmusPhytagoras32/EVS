// rooms.h
#ifndef ROOMS_H
#define ROOMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "rooms_struct.h"
#include "rooms.h"
#include "input.h"
#include "display.h"
#include "size_checker.h"

#define MAX_DIGIT 5
#define CHANGERATE 50

void createroom(struct room** head, struct room** tail, WINDOW* prompt_win);
void deleteroom(struct room** head, struct room** tail, WINDOW* prompt_win);
void editroom(struct room** head, struct room** tail, WINDOW* prompt_win);
void render(struct room* head, struct room* tail);
int getroom(struct room** head, struct room** tail, struct room** temp, char name[], WINDOW* prompt_win);
int countrooms(struct room* head, struct room* tail,  WINDOW* prompt_win);
int compare_rooms_desc(const void* a, const void* b);
void sortrooms(struct room** head, struct room** tail, WINDOW *prompt_win);
int getdistance(struct room* tofind, struct room* tail, int count, WINDOW* prompt_win);
void cleanrooms(struct room** head, struct room** tail, bool start, WINDOW* prompt_win);

// File I/O
void getsavedata(struct room** head, struct room** tail, WINDOW* prompt_win);
void recordsavedata(struct room* head, struct room* tail, WINDOW* prompt_win);


int SelectPrompt(WINDOW *win);
void Up2low(char *word);

#endif
