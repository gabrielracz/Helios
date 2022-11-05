#include "global.h"

void setup() {
  Serial.begin(BAUD);
  Serial.setTimeout(5);
}

union serialized_message srl = {0, 0};
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
    memset(&srl.data, 0, sizeof(srl.data));
    
    srl.msg.buffer[75] = 'X';
    srl.msg.buffer[0] = 0x77;
    srl.msg.buffer[126] = '0x66';
    srl.msg.len = sizeof(srl.data);  

    if(Serial.available() >= sizeof(struct message) - 2) { 
      int br = read_all(buf, sizeof(buf));
      memcpy(srl.data, buf, br);
      Serial.write(srl.data, sizeof(srl.data));
    }    
}
