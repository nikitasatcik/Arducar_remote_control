/*
    This is additional arduino slave board for using extra 6 PWM pins.
    It receives Joystick positions  x, y from the Master board.
     
    Created Aug 12, 2022, by Kolchiba Mykyta.
*/

#include <Wire.h>
#include <I2C_Anything.h> //https://github.com/nickgammon/I2C_Anything

const byte MY_ADDRESS = 8;
volatile boolean haveData = false;
volatile int x;
volatile int y;

void setup() {
  Wire.begin(MY_ADDRESS); // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void receiveEvent(int howMany) {
  if (howMany >= (sizeof x) + (sizeof y))
  {
    I2C_readAnything (x);
    I2C_readAnything (y);
    haveData = true;
  }  // end if have enough data
}

void loop() {
  if (haveData) {
    Serial.print("X = ");
    Serial.println(x);
    Serial.print("Y = ");
    Serial.println(y);
  }
}
