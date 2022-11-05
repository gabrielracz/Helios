#include "global.h"

void setup() {
	Serial.begin(BAUD);
	Serial.setTimeout(5);
}

Packet inc = {0, 0};

int read_all(char* buf, int buflen) {
	int br = 0;
	int tot = 0;
	while((br = Serial.readBytes(buf + tot, buflen - tot)) > 0){
		tot += br;
	}
	return tot;
}

void loop() {
	if(Serial.available() >= sizeof(Packet) - 2) { 

		memset(&inc.data, 0, sizeof(inc.data));
		int br = read_all(inc.data, sizeof(inc.data));

		Serial.write(inc.data, sizeof(inc.data));
	}    

}
