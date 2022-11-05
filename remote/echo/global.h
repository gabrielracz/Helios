#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>

#define BAUD 115200

struct message {
	char buf[31];
	uint8_t len;
};

enum CmdTypes {
	CMD_DELTA_POS,
	CMD_ABS_POS
};

struct command {
	uint8_t type;
	uint8_t val[3];
};

typedef union Packet_t{
	struct command cmd;
	char data[sizeof(struct command)];
}Packet;
#endif

