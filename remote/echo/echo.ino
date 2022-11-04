#include "global.h"

void setup() {
  Serial.begin(9600);
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

    if(Serial.available() > 62) { 
      // delay(10);
      int br = read_all(buf, sizeof(buf));
      memcpy(srl.data, buf, br);
      Serial.write(srl.data, sizeof(srl.data));
    }    

    // if(Serial.available() >= sizeof(srl.data)){
    //   delay(5);
    //   br = Serial.readBytes(srl.data, sizeof(srl.data));
    //   Serial.write(srl.data, sizeof(srl.data));
    // }
}


