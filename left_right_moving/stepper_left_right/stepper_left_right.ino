#include <Servo.h>
#include <Stepper.h>
#include <DistanceSensor.h>
// #include <StaticThreadController.h>
// #include <Thread.h>
// #include <ThreadController.h>

#define LEFT_PIN 2
#define RIGHT_PIN 3
#define PUNCH_PIN 4
#define PUNCH_SIGNAL 12

const int echoPin = 5;
const int trigPin = 6;

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor
// ThreadController controll = ThreadController();

// Thread thread1 = Thread();
// Thread thread2 = Thread();

// initialize the stepper library on pins 8 through 11:
Stepper BigStepper(stepsPerRevolution, 8, 9, 10, 11);
Servo myservo;
DistanceSensor sensor(trigPin, echoPin);

void setup() {
  // set the speed at 60 rpm:
  BigStepper.setSpeed(60);

  myservo.attach(7);
  myservo.write(0);
  
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(PUNCH_PIN, INPUT_PULLUP);
  pinMode(PUNCH_SIGNAL, OUTPUT);

  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(LEFT_PIN);
  int right = digitalRead(RIGHT_PIN);
  int punch = digitalRead(PUNCH_PIN);
  if (left == LOW && right == HIGH)
  {
    Serial.println("left");
    BigStepper.step(40);
  }
  else if (right == LOW && left == HIGH)
  {
    Serial.println("right");
    BigStepper.step(-40);
  }
  if (punch == LOW) {
    Serial.println("punch");
    myservo.write(180);
    delay(650);
    myservo.write(0);
    delay(1000);
  }
  int distance = sensor.getCM();
  if (distance < 7 && distance > 0)
  {
    Serial.println("SEND_PUNCH!");
    digitalWrite(12, HIGH);
  }
  else
    digitalWrite(12, LOW);
}