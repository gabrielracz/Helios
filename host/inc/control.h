#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct _Controller{
	int id;

} Controller;

static Controller ctrl;

int ctrl_init(const char* title, int w, int h);
int ctrl_update();

#endif
