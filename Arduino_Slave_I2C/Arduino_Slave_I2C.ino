#include <Wire.h>

char char_buffer[16] = "";

void setup() {
  Wire.begin(8); // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void receiveEvent(int howMany) {
  byte index = 0;
  while (Wire.available() and index < sizeof(char_buffer) - 1)
  {
    char_buffer[index++] = Wire.read();
    char_buffer[index] = '\0';
  }

  char x_buff[5];
  char y_buff[5];

  memcpy( x_buff, &char_buffer[0], 4 );
  x_buff[4] = '\0';
  int x;
  sscanf(x_buff, "%d", &x);
  Serial.print("X = ");
  Serial.println(x);

  memcpy( y_buff, &char_buffer[4], 4 );
  y_buff[4] = '\0';
  int y;
  sscanf(y_buff, "%d", &y);
  Serial.print("Y = ");
  Serial.println(y);
}



void loop() {
}
