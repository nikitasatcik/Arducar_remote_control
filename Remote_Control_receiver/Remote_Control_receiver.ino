#include <SPI.h>      //the communication interface with the modem
#include "RF24.h"     //the library which helps us to control the radio modem (nRF24L)
#include <DRV8833.h>

/* 
 *  Program for controlling motorized Lego Car with Arduino, DRV8833 and nRF24L01+.
 *  Arduino Nano Pin 11, 12, 13 reserved for SPI(MOSI, MISO, SCK) connection with nRF24.
 *
 *  Created July 29, 2022, by Kolchiba Mykyta.
*/

DRV8833 driver = DRV8833();

//Motor A is drive. Ain1 and Ain2 DRV8833 pins.
const int inputA1 = 9, inputA2 = 10;  //default pin 9, 10

//Motor B is Servo
const int servo_left = 7, servo_right = 8; // default pin 5, 6

int data[2];
int motorSpeed = 0;

RF24 radio(5, 4); // default pin 3,8: CE(PWM) and CSN pin

const uint64_t pipe = 0xE8E8F0F0E1LL; //the address of the modem

void setup() {
  Serial.begin(9600);

  // Attach a motor to the input pins:
  driver.attachMotorA(inputA1, inputA2);
  driver.attachMotorB(servo_left, servo_right);

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
      //digitalWrite(servo_left, HIGH);
      driver.motorBForward();//driver.motorBForward(255);
      Serial.println("TURN RIGHT");
      Serial.println(data[1]);
    }

    //data Y
    if (data[1] < 480 ) {
      //digitalWrite(servo_right, HIGH);
      driver.motorBReverse(); //driver.motorBReverse(255);
      Serial.println("TURN LEFT");
      Serial.println(data[1]);
    }

    if (data[0] < 525 && data[0] > 500 && data[1] < 550 && data[1] > 480) {
      driver.motorAStop();
      digitalWrite(servo_left, LOW);
      digitalWrite(servo_right, LOW);
      Serial.println("STOP");
      Serial.println(radio.getPALevel());
    }
  }
}
