// simulation.h
#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <ncurses.h>
#include "rooms_struct.h"
#include "simulation.h"
#include "rooms.h"
#include "input.h"
#include "display.h"

void runsimulation(WINDOW* menu_win, WINDOW* prompt_win, struct room** head, struct room** tail);

#endif
