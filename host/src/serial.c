#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>   //File attr defines
#include <assert.h>
#include <errno.h>
#include <termios.h>  
#include <sys/poll.h>

#include "../inc/defs.h"
#include "../inc/message.h"
#include "../inc/serial.h"

static int sfd;

int serial_quit() {
	close(sfd);
	return 0;
}

int serial_available() {
	struct pollfd poller = {0};
	poller.fd = sfd;
	poller.events = POLLIN;

	int rc = poll(&poller, 1, 0);
	return rc;
}

int serial_init(const char* dev_path) {
	int rc;
	struct termios tty;

	sfd = open(dev_path, O_RDWR);
	if(sfd < 0) {
		perror("serial fd open");
		return -errno;
	}

    /*int flags;*/
    /*flags = fcntl(sfd,F_GETFL,0);*/
    /*assert(flags != -1);*/
    /*fcntl(sfd, F_SETFL, flags | O_NONBLOCK);*/


	// SERIAL OPTIONS
	rc = tcgetattr(sfd, &tty); 
	if(rc) {
		perror("tcgetattr init_serial");
		return -errno;
	}

	//Control Modes
	tty.c_cflag &= ~PARENB;		// parity
	tty.c_cflag &= ~CSTOPB;		// one stop bit, set for two
	tty.c_cflag &= ~CSIZE;		// number of bits per byte, clear first
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;	// disable hardware flow control (RTS/CTS)
	tty.c_cflag |= CREAD;       // allow reading
	tty.c_cflag |= CLOCAL;      // disable signal control lines

	//Local Modes
	tty.c_lflag &= ~ICANON;     // canonical mode, if enable wait for \n and process backspace etc.
	tty.c_lflag &= ~ECHO;		// echo back characters
	tty.c_lflag &= ~ECHOE;		// echo back erasure
	tty.c_lflag &= ~ECHONL;		// echo back new lines
	tty.c_lflag &= ~ISIG;		// disable interpretation of INTR, QUIT, and SUSP chars

	// Input Modes
	// low level control of input processing before it gets to us
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);							 // software flow control
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // disable special handling of received bytes

	//Output Modes
	// low level control of output processing
	tty.c_oflag &= ~OPOST;		// special interpretation of output bytes e.g. \n
	tty.c_oflag &= ~ONLCR;      // conversion of \n to CRLF

	// VMIN = 0  VTIME = time-out from read() call
	// VMIN > 0  VTIME = time-out from first char recv
	// VMIN = 0  any number of chars
	// VMIN > 0  wait for this number of bytes
	//
	// VTIME is in deciseconds, (max 25.5 seconds)
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = sizeof(struct message);

	//Baud Rate
	
	cfsetspeed(&tty, B115200);    //set both input and output at once (linux fancy)

	rc = tcsetattr(sfd, TCSANOW, &tty);
	if(rc) {
		perror("tcsetattr init_serial");
		return -errno;
	}

	return 0;
}

/*int serial_send(struct message* msg) {*/
	/*int bs = write(sfd, msg->buffer, msg->len);*/
	/*return bs;*/
/*};*/
int serial_send(Packet* srl) {
	int bs = 0;
	int totb = 0;
	while((bs = write(sfd, srl->data + totb, sizeof(struct message) - totb)) > 0) {
		totb += bs;
	}
	return bs;
};

int serial_receive(Packet* p) {
	int br = 0;
	int totb = 0;
	while((br = read(sfd, p->data + totb, sizeof(p->data) - totb)) > 0) {
		totb += br;
	}
	return totb;
};
