#ifndef GLOBAL_H
#define GLOBAL_H
#define MSGSIZE 60
#define RSPSIZE 65

#include <stdint.h>

struct message {
	char buffer[63];
	uint8_t len;
};



union serialized_message{
	struct message msg;
	char data[sizeof(struct message)];
};
#endif

