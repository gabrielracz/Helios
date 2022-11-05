#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>

#define BAUD 115200

struct message {
	char buffer[31];
	uint8_t len;
};

union serialized_message{
	struct message msg;
	char data[sizeof(struct message)];
};
#endif

