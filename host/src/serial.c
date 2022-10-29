#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   //File attr defines
#include <errno.h>
#include <termios.h>  

/*#include "../defines.h"*/
#define MSGSIZE 60
#define RSPSIZE 65

int init_serial(int sfd) {
	int rc;
	struct termios tty;
	
	rc = tcgetattr(sfd, &tty); 
	if(rc) {
		perror("tcgetattr init_serial");
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
	tty.c_cc[VMIN] = 64;

	//Baud Rate
	
	cfsetspeed(&tty, B9600);    //set both input and output at once (linux fancy)

	rc = tcsetattr(sfd, TCSANOW, &tty);
	if(rc) {
		perror("tcsetattr init_serial");
	}

	return 0;
}

int serial_main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("USAGE: srl FILE\n");
		return 1;
	}

	int sfd = open("/dev/ttyUSB0", O_RDWR);
	if(sfd < 0) {
		perror("serial fd open");
	}
	init_serial(sfd);

	unsigned char recvbuf[RSPSIZE] = {0};
	char sendbuf[MSGSIZE];
	size_t bytes_read = 0;
	size_t bytes_sent = 0;
	while(1) {
		memset(sendbuf, 0, sizeof(sendbuf));
		fgets(sendbuf, 60, stdin);
		sendbuf[strlen(sendbuf)-1] = 0;
		bytes_sent = write(sfd, sendbuf, sizeof(sendbuf));
		bytes_read = read(sfd, recvbuf, sizeof(recvbuf));
		printf("%s\n", recvbuf);
	}
	close(sfd);

	return 0;
}
