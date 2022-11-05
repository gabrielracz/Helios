#include "global.h"

Packet inc = {0, 0};

void setup() {
	Serial.begin(BAUD);
	Serial.setTimeout(5);
	Serial.write(inc.data, sizeof(inc.data));
}


int read_all(char* buf, int buflen) {
	int br = 0;
	int tot = 0;
	while((br = Serial.readBytes(buf + tot, buflen - tot)) > 0){
		tot += br;
	}
	return tot;
}

void loop() {
	if(Serial.available() >= sizeof(Packet)) { 

		memset(&inc.data, 0, sizeof(Packet));
		int br = read_all(inc.data, sizeof(Packet));

		Serial.write(inc.data, sizeof(Packet));
	}    

}
