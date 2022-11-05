#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>

#define BAUD 115200

struct message {
	char buf[31];
	uint8_t len;
};

struct command {
	uint8_t type;
	uint8_t value;
};

typedef union Packet_t{
	struct message msg;
	char data[sizeof(struct message)];
}Packet;
#endif

