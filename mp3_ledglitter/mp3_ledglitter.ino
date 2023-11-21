#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>


SoftwareSerial MP3Module(10, 11);
DFRobotDFPlayerMini MP3Player;

Thread glitter_thread = Thread();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  MP3Module.begin(9600);
  if (!MP3Player.begin(MP3Module)) {
    Serial.println(F("Unalbe to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card"));
    while (true);
  }
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  //glitter_thread.onRun(glitter);
  delay(1);
  MP3Player.volume(20);
  MP3Player.play(3);
}

void glitter() {
  int tmp;
  for (int i = 0; i < 5; i++) {
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    tmp = millis();
    while (tmp + 100 >= millis());
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    tmp = millis();
    while (tmp + 100 >= millis());
  }
}

void loop() {
  if (!digitalRead(2)) {
    MP3Player.advertise(4);
    //glitter_thread.run();
    //delay(1000);
  }
  if (!digitalRead(3)) {
    MP3Player.advertise(5);
    //glitter_thread.run();
    //delay(1000);
  }
  if (!digitalRead(4)) {
    MP3Player.advertise(6);
    //glitter_thread.run();
    //delay(1000);
  }
  if (!digitalRead(5)) {
    MP3Player.advertise(7);
    //glitter_thread.run();
    //delay(1000);
  }
  delay(10);
}
