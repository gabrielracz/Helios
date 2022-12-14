#include "global.h"

void setup() {
	Serial.begin(BAUD);
	Serial.setTimeout(5);
}

Packet srl = {0, 0};
char buf[256];

int read_all(char* buf, int buflen) {
	int br = 0;
	int tot = 0;
	while((br = Serial.readBytes(buf + tot, buflen - tot)) > 0){
		tot += br;
	}
	return tot;
}

void loop() {

	if(Serial.available() >= sizeof(struct message) - 2) { 
		memset(&srl.data, 0, sizeof(srl.data));
		int br = read_all(srl.data, sizeof(srl.data));
		Serial.write(srl.data, sizeof(srl.data));
	}    

}
