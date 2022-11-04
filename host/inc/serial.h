#ifndef SERIAL_H
#define SERIAL_H

#include "../inc/message.h"
#include "../../global.h"

int serial_available();
int serial_init(const char* dev_path);

//int serial_send(struct message* msg);
int serial_send(union serialized_message* msg);

//int serial_receive(struct message* msg);
int serial_receive(union serialized_message* msg);

int serial_quit();

#endif
