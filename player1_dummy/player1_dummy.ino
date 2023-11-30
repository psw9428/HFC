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
//      STEPPER_PIN 8, 9, 10, 11
#define GUARD_SIGNAL 12
#define PUNCH_SIGNAL 3
#define DAMAGE_SIGNAL A2

// Using in punch_func
#define READY_POSE 0
#define ON_PUNCHING 1
#define ON_PUNCHBACK 2
#define PUNCH_DELAY 1000
#define PUNCH_DEFAULT_ANGLE 40

// Using in distance_func
#define OUT_OF_RANGE 0
#define IN_PUNCH_RANGE 1
#define TOO_CLOSE 2
#define NO_DASH 4

// Using in left_right_func
#define LEFT 1
#define RIGHT 2
#define MOVE_SPEED 15
#define DASH_SPEED 40
#define DASH_DISTANCE 30

const int stepsPerRevolution = 200;

ThreadController controll = ThreadController();

// make thread object
Thread left_right_thread = Thread();
Thread punch_thread = Thread();
Thread distance_thread = Thread();
Thread guard_thread = Thread();
// Thread damaged_thread = Thread();

// make Stepper, DistanceSensor object
Stepper BigStepper(stepsPerRevolution, 8, 9, 10, 11);
Servo myservo;
DistanceSensor sensor(TRIGPIN, ECHOPIN);

int distance_status;
bool guard_status;
bool damaged_status;

void setup() {
  Serial.begin(9600);
  // initialize function
  initial();
}

void initial() {
  // set stepper speed
  BigStepper.setSpeed(MOVE_SPEED);

  // servo reset (punch motor)
  myservo.attach(SERVO_PIN, 444, 2500);
  myservo.write(PUNCH_DEFAULT_ANGLE);

  // left, right moving and punch pin setting
  pinMode(PUNCH_PIN, INPUT_PULLUP);
  pinMode(GUARD_PIN, INPUT_PULLUP);
  pinMode(DASH_PIN, INPUT_PULLUP);
  pinMode(PUNCH_SIGNAL, OUTPUT);
  pinMode(GUARD_SIGNAL, OUTPUT);

  // thread onRun setting
  left_right_thread.onRun(left_right_func);
  punch_thread.onRun(punch_func);
  distance_thread.onRun(distance_func);
  guard_thread.onRun(guard_func);
  //damaged_thread.onRun(damaged_func);

  // set thread's interval
  distance_thread.setInterval(50);

  // thread controller setting
  controll.add(&left_right_thread);
  controll.add(&punch_thread);
  controll.add(&distance_thread);
  controll.add(&guard_thread);

  digitalWrite(PUNCH_SIGNAL, LOW);
  digitalWrite(GUARD_SIGNAL, LOW);

  delay(1000);
  while(digitalRead(DAMAGE_SIGNAL));

  // initialize global var
  distance_status = 0;
  guard_status = false;
  damaged_status = false;
}

// manage moving and dash
void left_right_func() {
  // dameged_status : stunning, guarding : can't move
  if (guard_status) 
    return;

  int joystick;
  static int status = 0;
  static unsigned long tmp = 0;

  joystick = analogRead(A0);

  // move part
  if (joystick > 550) {
    BigStepper.step(1);
    status = LEFT ;
  }
  else if (joystick < 470 && !(distance_status & TOO_CLOSE)) {
    BigStepper.step(-1);
    status = RIGHT;
  }
  else
    status = 0;
  
  // dash part
  if (!digitalRead(DASH_PIN) && tmp + 800 < millis()) {
    BigStepper.setSpeed(DASH_SPEED);
    BigStepper.step(DASH_DISTANCE * (status == LEFT) - DASH_DISTANCE * (status == RIGHT));
    BigStepper.setSpeed(MOVE_SPEED);
    tmp = millis();
  }
}

// manage punch action
void punch_func() {
  // if damaged exit func
  // if (damaged_status)
  //   return;

  static unsigned long time = 0;
  static int status = 0;

  // punch part
  if (guard_status)
    status = 0;
  else if (!digitalRead(PUNCH_PIN) && !status) {
    //Serial.println("punch");
    myservo.write(PUNCH_DEFAULT_ANGLE + 60);
    time = millis();
    status = ON_PUNCHING;
  }
  if (time + 350 <= millis() && status & ON_PUNCHING) {
    status = ON_PUNCHBACK;
    if(distance_status & IN_PUNCH_RANGE) {
      digitalWrite(PUNCH_SIGNAL, HIGH);
      //Serial.println("SEND_PUNCH_SIGNAL");
    }
    myservo.write(PUNCH_DEFAULT_ANGLE);
    time = millis();
  }
  // End the signal earlier than punchback
  if (time + 100 <= millis() && status & ON_PUNCHBACK)
    digitalWrite(PUNCH_SIGNAL, LOW);
  // Here is the time when punch end
  if (time + PUNCH_DELAY <= millis() && status & ON_PUNCHBACK) {
    digitalWrite(PUNCH_SIGNAL, LOW);
    status = READY_POSE;
  }
}

// manage distance_status
void distance_func() {
  // measure distance
  float distance = sensor.getCM();

  if (distance <= 3.0 && distance > 0 && !(distance_status & TOO_CLOSE)) {
    //Serial.println("TOO_CLOSE!");
    distance_status = TOO_CLOSE | IN_PUNCH_RANGE;
  }
  else if (distance > 3.0 && distance <= 4.0 && !(distance_status & IN_PUNCH_RANGE)) {
    //Serial.println("IN_PUNCH_RANGE!");
    distance_status = IN_PUNCH_RANGE;
  }
  else if (distance > 4.0 && distance_status) {
    //Serial.println("OUT_RANGE");
    distance_status = OUT_OF_RANGE;
  }
}

// manage guard status
void guard_func() {
  // if damaged, exit this func
  // if (damaged_status)
  //   return;
  
  // guard part
  if (!digitalRead(GUARD_PIN)) {
    if (!guard_status) {
      //Serial.println("GUARD!");
      guard_status = true;
      digitalWrite(GUARD_SIGNAL, HIGH);
      myservo.write(PUNCH_DEFAULT_ANGLE - 30);
    }
  }
  else if (guard_status) {
    //Serial.println("NO_GUARD");
    guard_status = false;
    digitalWrite(GUARD_SIGNAL, LOW);
    myservo.write(PUNCH_DEFAULT_ANGLE);
  }
}

// manage damaged(stun) status
void damaged_func() {
  static unsigned long tmp = 0;

  if (digitalRead(DAMAGE_SIGNAL) && !damaged_status) {
    //Serial.println("DAMAGED..! STUNNING");
    damaged_status = true;
    tmp = millis();
    myservo.write(PUNCH_DEFAULT_ANGLE - 30);
  }
  if (damaged_status && tmp + 500 <= millis()) {
    damaged_status = false;
    tmp = 0;
    myservo.write(PUNCH_DEFAULT_ANGLE);
    //Serial.println("STUN END");
  }
}

void loop() {
  controll.run();
}