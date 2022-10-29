#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "../inc/control.h"
#include "../inc/message.h"
#include "../inc/defs.h"

int main(int argc, char* argv[]) {

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	printf("%s\n", cwd);

	int rc;
	rc = ctrl_init("Helios", 500, 500);
	if(rc) {
		return 1;
	}

	while(1) {
		rc = ctrl_update();
		if(rc == CLOSE) {
			break;
		}
	}

	return 0;
}
