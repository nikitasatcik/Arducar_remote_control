//#include <SPI.h>
#include <DRV8833.h>
#include <Servo.h>
#include "RF24.h"

/*
    Program for controlling motorized Lego Car with Arduino, DRV8833 and nRF24L01+.
    Arduino Nano Pin 11, 12, 13 reserved for SPI(MOSI, MISO, SCK) connection with nRF24.

    Created July 29, 2022, by Kolchiba Mykyta.
*/

DRV8833 driver = DRV8833(); // Motor A is drive.
Servo myservo; // Motor B is Servo
const int inputA1 = 5, inputA2 = 6; // Ain1 and Ain2 DRV8833 pins.



int data[2];
int motorSpeed = 0;
int pos = 90;    // Servo position

RF24 radio(7, 8); // CE and CSN pin

const uint64_t pipe = 0xE8E8F0F0E1LL; //the address of the modem

void setup() {
  Serial.begin(9600);

  // Attach a motor to the input pins:
  driver.attachMotorA(inputA1, inputA2);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  radio.begin();
  /* Settings*/
  radio.setChannel(125); // 2400 MHz + channel
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, pipe);  //determines the address of our modem which receive data
  radio.startListening();
}


void loop() {
  if (radio.available()) {
    radio.read(data, sizeof(data));

    //data X
    if (data[0] > 525) {
      motorSpeed = map(data[0], 512, 1023, 0, 255);
      driver.motorAForward(motorSpeed);
      Serial.println("FORWARD");
      Serial.println(data[0]);
      Serial.print("speed:");
      Serial.println(motorSpeed);
    }

    //data X
    if (data[0] < 500) {
      motorSpeed = map(data[0], 512, 0, 0, 255);
      driver.motorAReverse(motorSpeed);
      Serial.println("BACKWARD");
      Serial.println(data[0]);
    }

    //data Y
    if (data[1] > 550 ) {
      pos = map(data[1], 512, 1023, 90, 180);
      myservo.write(pos);
      Serial.println("TURN RIGHT");
      Serial.println(data[1]);
    }

    //data Y
    if (data[1] < 480 ) {
      pos = map(data[1], 512, 0, 90, 0);
      myservo.write(pos);
      Serial.println("TURN LEFT");
      Serial.println(data[1]);
    }

    if (data[0] < 525 && data[0] > 500 && data[1] < 550 && data[1] > 480) {
      driver.motorAStop();
      pos = map(data[1], 480, 550, 80, 100);
      myservo.write(pos);
      Serial.println("STOP");
      //Serial.println(radio.getPALevel());
    }

  }
}
