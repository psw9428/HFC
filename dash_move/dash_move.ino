#include <Servo.h>
#include <Stepper.h>
#include <DistanceSensor.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define DEFAULT 0

#define LEFT_PIN 2
#define RIGHT_PIN 3
#define PUNCH_PIN 4
#define PUNCH_SIGNAL 12

#define MOVE_SPEED 20
#define DASH_SPEED 80

#define ECHOPIN 5
#define TRIGPIN 6

#define READY_POSE 0
#define ON_PUNCHING 1
#define ON_PUNCHBACK 2

#define OUT_OF_RANGE 0
#define IN_OF_RANGE 1

#define PUNCH_DELAY 1000

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor
ThreadController controll = ThreadController();

Thread left_right_thread = Thread();
//Thread punch_thread = Thread();
//Thread distance_thread = Thread();

// initialize the stepper library on pins 8 through 11:
Stepper BigStepper(stepsPerRevolution, 8, 9, 10, 11);
//Servo myservo;
//DistanceSensor sensor(TRIGPIN, ECHOPIN);

void setup() {
  BigStepper.setSpeed(MOVE_SPEED);

  //myservo.attach(7, 444, 2500);
  //myservo.write(0);
  
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  //pinMode(PUNCH_PIN, INPUT_PULLUP);
  //pinMode(PUNCH_SIGNAL, OUTPUT);

  left_right_thread.onRun(left_right_func);
  //punch_thread.onRun(punch_func);
  //distance_thread.onRun(distance_func);
  //distance_thread.setInterval(50);
  controll.add(&left_right_thread);
  //controll.add(&punch_thread);
  //controll.add(&distance_thread);

  // initialize the serial port:
  Serial.begin(9600);
}

void left_right_func() {
  int left;
  int right;
  static int status = 0;
  static int dash_status = 0;
  static unsigned long tmp = 0;
  static unsigned long prev_touch = 0;
  left = digitalRead(LEFT_PIN);
  right = digitalRead(RIGHT_PIN);
  if (!left && right) {
    BigStepper.step(1);
    if (!tmp)
      tmp = millis();
    status = LEFT_PIN;
  }
  else if (left && !right) {
    BigStepper.step(-1);
    status = RIGHT_PIN;
    if (!tmp)
      tmp = millis();
  }
  else if (status) {
    if (millis() - tmp <= 150) { // touch very short
      Serial.println("SHORT_TOUCH");
      if (status == RIGHT_PIN && dash_status == RIGHT_PIN && millis() - prev_touch <= 400) {
        Serial.println("DASH_RIGHT!");
        BigStepper.setSpeed(DASH_SPEED);
        BigStepper.step(-100);
        dash_status = DEFAULT;
        prev_touch = 0;
        BigStepper.setSpeed(MOVE_SPEED);
      }
      else if (status == LEFT_PIN && dash_status == LEFT_PIN && millis() - prev_touch <= 400) {
        Serial.println("DASH_LEFT!!");
        BigStepper.setSpeed(DASH_SPEED);
        BigStepper.step(100);
        dash_status = DEFAULT;
        prev_touch = 0;
        BigStepper.setSpeed(MOVE_SPEED);
      }
      else if (dash_status == DEFAULT) {
        dash_status = status;
        prev_touch = millis();
      }
      else {
        dash_status = DEFAULT;
        prev_touch = 0;
      }
    }
    status = DEFAULT;
    tmp = 0;
  }
}

// void punch_func() {
//   static unsigned long time = 0;
//   static int status = 0;

//   if (!digitalRead(PUNCH_PIN) && status == READY_POSE) {
//     Serial.println("punch");
//     myservo.write(180);
//     time = millis();
//     status = ON_PUNCHING;
//   }
//   if (time + 600 <= millis() && status == ON_PUNCHING) {
//     status = ON_PUNCHBACK;
//     myservo.write(0);
//     time = millis();
//   }
//   if (time + PUNCH_DELAY <= millis() && status == ON_PUNCHBACK)
//     status = READY_POSE;
// }

// void distance_func() {
//   int distance = sensor.getCM();
//   static int status = 0;

//   if (distance < 7 && distance > 0 && status == OUT_OF_RANGE) {
//     Serial.println("IN_RANGE!");
//     status = IN_OF_RANGE;
//     digitalWrite(PUNCH_SIGNAL, HIGH);
//   }
//   else if (status == IN_OF_RANGE && distance > 7) {
//     Serial.println("OUT_RANGE!");
//     status = OUT_OF_RANGE;
//     digitalWrite(PUNCH_SIGNAL, LOW);
//   }
// }

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