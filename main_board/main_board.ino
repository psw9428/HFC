#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define P1_PUNCH_SIGNAL A0
#define P1_GUARD_SIGNAL 2
#define P1_DAMAGE_SIGNAL 3

#define P2_PUNCH_SIGNAL A3
#define P2_GUARD_SIGNAL 8
#define P2_DAMAGE_SIGNAL 9

#define P1_SEND_LCD_PUNCH 10
#define P2_SEND_LCD_PUNCH 11

#define DAMAGE 10

#define PUNCHING 1
#define GUARDING 2
#define STUN 4


SoftwareSerial MP3Module(10, 11);
DFRobotDFPlayerMini MP3Player;


struct player {
  int hp;
  int status;
};

player p1 = {100, 0};
player p2 = {100, 0};

ThreadController controll = ThreadController();

Thread p1_thread = Thread();
Thread p2_thread = Thread();

void setup() {
  Serial.begin(9600);
  MP3Module.begin(9600);
  // if (!MP3Player.begin(MP3Module)) {
  //   Serial.println(F("Unalbe to begin:"));
  //   Serial.println(F("1. Please recheck the connection!"));
  //   Serial.println(F("2. Please insert the SD card"));
  //   while (true);
  // }

  pinMode(P1_PUNCH_SIGNAL, INPUT);
  pinMode(P1_GUARD_SIGNAL, INPUT);
  pinMode(P1_DAMAGE_SIGNAL, OUTPUT);
  pinMode(P1_SEND_LCD_PUNCH, OUTPUT);

  pinMode(P2_PUNCH_SIGNAL, INPUT);
  pinMode(P2_GUARD_SIGNAL, INPUT);
  pinMode(P2_DAMAGE_SIGNAL, OUTPUT);
  pinMode(P2_SEND_LCD_PUNCH, OUTPUT);

  digitalWrite(P2_SEND_LCD_PUNCH, LOW);

  p1_thread.onRun(p1_func);
  p2_thread.onRun(p2_func);

  controll.add(&p1_thread);
  controll.add(&p2_thread);
  
  MP3Player.volume(15);
  MP3Player.play(3);
}

void p1_func() {
  static unsigned long tmp = 0;
  static bool stun = false;
  if (stun && tmp + 400 >= millis())
    return;
  if (stun) {
    digitalWrite(P1_SEND_LCD_PUNCH, LOW);
    digitalWrite(P1_DAMAGE_SIGNAL, LOW);
    stun = false;
  }
  if (digitalRead(P1_GUARD_SIGNAL))
    p1.status = GUARDING;
  else if (digitalRead(P1_PUNCH_SIGNAL))
    p1.status = PUNCHING;
  else
    p1.status = 0;
  if (!stun && !(p1.status & GUARDING) && (p2.status & PUNCHING)) {
    p1.hp -= DAMAGE;
    p1.status = STUN;
    stun = true;
    digitalWrite(P1_SEND_LCD_PUNCH, HIGH);
    digitalWrite(P1_DAMAGE_SIGNAL, HIGH);
    Serial.println("punch!!");
    tmp = millis();
  }
}

void p2_func() {
  static unsigned long tmp = 0;
  static bool stun = false;
  if (stun && tmp + 400 >= millis())
    return;
  if (stun) {
    digitalWrite(P2_SEND_LCD_PUNCH, LOW);
    digitalWrite(P2_DAMAGE_SIGNAL, LOW);
    stun = false;
  }
  if (digitalRead(P2_GUARD_SIGNAL))
    p2.status = GUARDING;
  else if (digitalRead(P2_PUNCH_SIGNAL))
    p2.status = PUNCHING;
  else
    p2.status = 0;
  if (!stun && !(p2.status & GUARDING) && (p1.status & PUNCHING)) {
    p2.hp -= DAMAGE;
    p2.status = STUN;
    stun = true;
    digitalWrite(P2_SEND_LCD_PUNCH, HIGH);
    digitalWrite(P2_DAMAGE_SIGNAL, HIGH);
    tmp = millis();
  }
}

void loop() {
  controll.run();
}

