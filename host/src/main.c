#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "../inc/control.h"
#include "../inc/message.h"
#include "../inc/serial.h"
#include "../inc/defs.h"
#include "../inc/renderer.h"


int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("Usage: helios PORT\n");
		return -1;
	}

	if(rndr_init("Helios", 750, 250))	goto quit;
	if(ctrl_init())						goto quit;
	if(serial_init(argv[1]))			goto quit;

	int rc;
	while(1) {
		rc = ctrl_update();
		if(rc == CLOSE) {
			break;
		}

		rc = rndr_update();
		if(rc == CLOSE) {
			break;
		}
	}

quit:
	ctrl_quit();
	rndr_quit();
	serial_quit();
	return 0;
}
