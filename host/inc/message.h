#ifndef MESSAGER_H
#define MESSAGER_H

struct message {
	char buffer[60];
	int len;
};


int msg_send(struct message*);
int msg_receive(struct message*);


#endif
