/*
    Program for controlling motorized Lego Car with Arduino, DRV8833 and nRF24L01+.
    Arduino Nano Pin 11, 12, 13 reserved for SPI(MOSI, MISO, SCK) connection with nRF24.

    Created July 29, 2022, by Kolchiba Mykyta.
*/

#include <SPI.h>
#include <Wire.h>
#include "RF24.h"
#include <I2C_Anything.h>

const byte SLAVE_ADDRESS = 8;
int data[2];
int motorSpeed = 0;

RF24 radio(3, 4); // default pin 3,8: CE(PWM) and CSN pin

const uint64_t pipe = 0xE8E8F0F0E1LL; //the address of the modem

void setup() {
  Wire.begin(); // join i2c bus with address #8
  Serial.begin(9600);

  radio.begin();
  /* Settings*/
  radio.setChannel(125); // 2400 MHz + channel
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, pipe);  //determines the address of our modem which receive data
  radio.startListening();
}

void send_by_i2c(int first_val, int second_val)
{
  Wire.beginTransmission(SLAVE_ADDRESS); // transmit to device #8
  I2C_writeAnything (first_val);
  I2C_writeAnything (second_val);
  Wire.endTransmission();    // stop transmitting
}

void loop() {
  if (radio.available()) {
    radio.read(data, sizeof(data));
  }
  send_by_i2c(data[0], data[1]);
}
