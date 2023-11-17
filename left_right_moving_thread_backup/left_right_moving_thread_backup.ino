#include <Servo.h>
#include <Stepper.h>
//#include <StepperMulti.h>
#include <DistanceSensor.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define LEFT_PIN 2
#define RIGHT_PIN 3
#define PUNCH_PIN 4
#define PUNCH_SIGNAL 12

#define ECHOPIN 5;
#define TRIGPIN 6;

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor
ThreadController controll = ThreadController();

Thread left_right_thread = Thread();
Thread punch_thread = Thread();

// initialize the stepper library on pins 8 through 11:
Stepper BigStepper(stepsPerRevolution, 8, 9, 10, 11);
Servo myservo;
//DistanceSensor sensor(trigPin, echoPin);

void setup() {
  // set the speed at 60 rpm:
  BigStepper.setSpeed(60);

  myservo.attach(7);
  myservo.write(0);
  
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(PUNCH_PIN, INPUT_PULLUP);
  //pinMode(PUNCH_SIGNAL, OUTPUT);

  left_right_thread.onRun(left_right_func);
  punch_thread.onRun(punch_func);
  controll.add(&left_right_thread);
  controll.add(&punch_thread);

  //controll.run();
  // initialize the serial port:
  Serial.begin(9600);
}

void left_right_func() {
  int left;
  int right;
  left = digitalRead(LEFT_PIN);
  right = digitalRead(RIGHT_PIN);
  if (!left && right) {
    Serial.println("left");
    BigStepper.step(1);
  }
  else if (left && !right) {
    Serial.println("right");
    BigStepper.step(-1);
  }
}

void punch_func() {
  static int i = 0;
  static int punch = 0;
  if (i > 0) {
    i--;
    delay(1);
  }
  else if (i == 0 && punch) {
    punch = 0;
    myservo.write(0);
    i = 1000;
  }
  else if (!digitalRead(PUNCH_PIN)) {
    Serial.println("punch");
    myservo.write(180);
    i = 650;
    punch = 1;
  }
}

void loop() {
  // int distance = sensor.getCM();
  // if (distance < 7 && distance > 0)
  // {
  //   Serial.println("SEND_PUNCH!");
  //   digitalWrite(12, HIGH);
  // }
  // else
  //   digitalWrite(12, LOW);
  controll.run();
}