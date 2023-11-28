#include <Servo.h>
#include <Stepper.h>
#include <DistanceSensor.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define DASH_PIN 2
#define GUARD_PIN 13
#define PUNCH_PIN 4

#define ECHOPIN 5
#define TRIGPIN 6

#define SERVO_PIN 7
// 8, 9, 10, 11 : Stepper
#define GUARD_SIGNAL 12
#define PUNCH_SIGNAL 3
#define DAMAGE_SIGNAL A2

#define MOVE_SPEED 10
#define DASH_SPEED 50
#define DASH_DISTANCE 40

#define READY_POSE 0
#define ON_PUNCHING 1
#define ON_PUNCHBACK 2

#define OUT_OF_RANGE 0
#define IN_PUNCH_RANGE 1
#define TOO_CLOSE 2
#define NO_DASH 4

#define LEFT 1
#define RIGHT 2

#define PUNCH_DELAY 1000

const int stepsPerRevolution = 200;  
ThreadController controll = ThreadController();

Thread left_right_thread = Thread();
Thread punch_thread = Thread();
Thread distance_thread = Thread();
Thread guard_thread = Thread();
Thread damaged_thread = Thread();

Stepper BigStepper(stepsPerRevolution, 8, 9, 10, 11);
Servo myservo;
DistanceSensor sensor(TRIGPIN, ECHOPIN);

int distance_status;
bool guard_status;
bool damaged_status;

void setup() {
  // initialize function
  initial();

  Serial.begin(9600);
}

void initial() {
  // set stepper speed
  BigStepper.setSpeed(MOVE_SPEED);

  // servo reset (punch motor)
  myservo.attach(SERVO_PIN, 444, 2500);
  myservo.write(10);

  // left, right moving and punch pin setting
  pinMode(PUNCH_PIN, INPUT_PULLUP);
  pinMode(GUARD_PIN, INPUT_PULLUP);
  pinMode(DASH_PIN, INPUT_PULLUP);
  pinMode(PUNCH_SIGNAL, OUTPUT);

  // thread onRun setting
  left_right_thread.onRun(left_right_func);
  punch_thread.onRun(punch_func);
  distance_thread.onRun(distance_func);
  guard_thread.onRun(guard_func);
  //damaged_thread.onRun();

  // set thread's interval
  distance_thread.setInterval(50);

  // thread controller setting
  controll.add(&left_right_thread);
  controll.add(&punch_thread);
  controll.add(&distance_thread);
  controll.add(&guard_thread);

  digitalWrite(PUNCH_SIGNAL, LOW);

  // initialize global var
  distance_status = 0;
  guard_status = false;
  damaged_status = false;
}

void left_right_func() {
  int joystick;
  static int status = 0;
  static unsigned long tmp = 0;
  joystick = analogRead(A0);

  if (guard_status)
    return;
  if (joystick < 450) {
    BigStepper.step(-1);
    status = LEFT ;
  }
  else if (joystick > 550 && !(distance_status & TOO_CLOSE)) {
    BigStepper.step(1);
    status = RIGHT;
  }
  else
    status = 0;
  if (!digitalRead(DASH_PIN) && tmp + 800 < millis()) {
    BigStepper.setSpeed(DASH_SPEED);
    BigStepper.step(DASH_DISTANCE * (status == RIGHT) - DASH_DISTANCE * (status == LEFT));
    BigStepper.setSpeed(MOVE_SPEED);
    tmp = millis();
  }
}

void punch_func() {
  static unsigned long time = 0;
  static int status = 0;

  if (!digitalRead(PUNCH_PIN) && !status) {
    if (guard_status)
      return;
    Serial.println("punch");
    myservo.write(140);
    time = millis();
    status = ON_PUNCHING;
  }
  if (time + 500 <= millis() && status & ON_PUNCHING) {
    status = ON_PUNCHBACK;
    if(distance_status & IN_PUNCH_RANGE)
      digitalWrite(PUNCH_SIGNAL, HIGH);
    myservo.write(10);
    time = millis();
  }
  if (time + 100 <= millis() && status & ON_PUNCHBACK)
    digitalWrite(PUNCH_SIGNAL, LOW);
  if (time + PUNCH_DELAY <= millis() && status & ON_PUNCHBACK) {
    digitalWrite(PUNCH_SIGNAL, LOW);
    status = READY_POSE;
  }
}

void distance_func() {
  float distance = sensor.getCM();

  if (distance <= 3.0 && distance > 0 && !(distance_status & TOO_CLOSE)) {
    //Serial.print(distance);
    Serial.println("TOO_CLOSE!");
    distance_status = TOO_CLOSE | IN_PUNCH_RANGE;
  }
  else if (distance > 3.0 && distance <= 4.0 && !(distance_status & IN_PUNCH_RANGE)) {
    //Serial.print(distance);
    Serial.println("IN_PUNCH_RANGE!");
    distance_status = IN_PUNCH_RANGE;
  }
  else if (distance > 4.0 && distance_status) {
    //Serial.print(distance);
    Serial.println("OUT_RANGE");
    distance_status = OUT_OF_RANGE;
  }
}

void guard_func() {
  // if (!digitalRead(GUARD_PIN)) {
  //   if (!guard_status) {
  //     Serial.println("GUARD!");
  //     guard_status = true;
  //     digitalWrite(GUARD_SIGNAL, HIGH);
  //   }
  // }
  // else if (guard_status) {
  //   Serial.println("NO_GUARD");
  //   guard_status = false;
  //   digitalWrite(GUARD_SIGNAL, LOW);
  // }
}

void damaged_func() {
  // static unsigned long tmp = 0;
  // if (digitalRead(DAMAGED_SIGNAL) && !damaged_status) {
  //   damaged_status = true;
  //   tmp = millis();
  //   myservo.write(0);
  // }
  // if (damaged_status && tmp + 500 <= millis()) {
  //   damaged_status = false;
  //   tmp = 0;
  //   myservo.write(10);
  // }
}

void loop() {
  controll.run();
}