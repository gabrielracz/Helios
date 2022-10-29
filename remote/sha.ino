#include "libsha.h"
#include "../defines.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

const int input_len = 64 - 4;
char output[256/8 + 1] = {0};
char input_buffer[input_len] = {0};
unsigned long start, end;

void loop() {
  if(Serial.available() >= MSGSIZE) {
    Serial.readBytes(input_buffer, MSGSIZE);

    sha256(input_buffer, strlen(input_buffer), output);

    Serial.write(output, RSPSIZE);
    memset(input_buffer, 0, input_len);
  }
}
