#include <Servo.h>
#include "global.h"

struct State {
	uint8_t x_axis;
	uint8_t y_axis;
	int8_t x_inv;
	int8_t y_inv;
};

struct State state;

Packet dummy = {0};

Servo servox;
Servo servoy;

uint8_t clampbyte(int n) {
	return (n < 0) ? 0 : (n > 255) ? 255 : n;
}

uint8_t clampangle(int a) {
	return (a < 0) ? 0 : (a > 180) ? 180 : a;
}

int parse_packet(Packet* p) {
	switch(p->cmd.type) {
	case CMD_DELTA_POS:
		state.x_axis = clampangle(state.x_axis + (p->cmd.val[0] - 127) * state.x_inv);
		state.y_axis = clampangle(state.y_axis + (p->cmd.val[1] - 127) * state.y_inv);
		break;
	}
	
	return 0;
}

void update_state() {
	servox.write(state.x_axis);
	servoy.write(state.y_axis);
}


int read_all(char* buf, int buflen) {
	int br = 0;
	int tot = 0;
	while((br = Serial.readBytes(buf + tot, buflen - tot)) > 0){
		tot += br;
	}
	return tot;
}

void setup() {
	Serial.begin(BAUD);
	Serial.setTimeout(5);
	Serial.write(dummy.data, sizeof(Packet));
	state.x_axis = 90;
	state.y_axis = 90;
	state.x_inv = -1;
	state.y_inv = 1;


	servox.attach(10);
	servoy.attach(11);
}

void loop() {
	if(Serial.available() >= sizeof(Packet)) { 
		Packet inc = {0};
		int br = read_all(inc.data, sizeof(Packet));
		parse_packet(&inc);
		update_state();
		Packet out;
		out.cmd.type   = CMD_DELTA_POS;
		out.cmd.val[0] = state.x_axis;
		out.cmd.val[1] = state.y_axis;
		out.cmd.val[2] = 0;
		Serial.write(out.data, sizeof(Packet));
	}    

}
