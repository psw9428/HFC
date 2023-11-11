#include <Stepper.h>

/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(30);
  // initialize the serial port:
  pinMode(12, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int a = digitalRead(12);
  int b = digitalRead(7);
  if (a == LOW && b == HIGH)
    myStepper.step(20);
  else if (b == LOW && a == HIGH)
    myStepper.step(-20);


  // // step one revolution  in one direction:
  // Serial.println("clockwise");
  // myStepper.step(stepsPerRevolution);
  // delay(500);

  // // step one revolution in the other direction:
  // Serial.println("counterclockwise");
  // myStepper.step(-stepsPerRevolution);
  // delay(500);
}

