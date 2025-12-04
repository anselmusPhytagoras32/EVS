#ifndef ROOM_STRUCT_H
#define ROOM_STRUCT_H

struct room
{
	struct room *nextnode;
	struct room *prevnode;
	struct room *evacroom;
	char name[16];
	int pop_max;
	int pop_start;
	int pop_current;
	int pop_send_limit;
	int pop_received;
	int dist;
};

#endif
