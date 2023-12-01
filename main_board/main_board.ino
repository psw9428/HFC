#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define P1_PUNCH_SIGNAL A0
#define P1_GUARD_SIGNAL 2
#define P1_DAMAGE_SIGNAL 3

#define P2_PUNCH_SIGNAL A5
#define P2_GUARD_SIGNAL 8
#define P2_DAMAGE_SIGNAL 9

#define P1_SEND_LCD_PUNCH 10
#define P2_SEND_LCD_PUNCH 11

#define P1_END_PIN 13
#define P2_END_PIN 12

#define DAMAGE 10

#define PUNCHING 1
#define GUARDING 2
#define STUN 4
#define END 8
#define LOOSE 16
#define WIN 32

#define DAMAGE_DELAY 600

#define MAX_HEALTH 80
#define MUSIC_VOLUME 15


SoftwareSerial MP3Module(5, 6);
DFRobotDFPlayerMini MP3Player;


struct player {
  int hp;
  int status;
};

player p1 = {MAX_HEALTH, 0};
player p2 = {MAX_HEALTH, 0};

ThreadController controll = ThreadController();

Thread p1_thread = Thread();
Thread p2_thread = Thread();

void setup() {
  Serial.begin(9600);
  MP3Module.begin(9600);
  if (!MP3Player.begin(MP3Module), false) {
    Serial.println(F("Unalbe to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card"));
    while (true);
  }

  pinMode(P1_PUNCH_SIGNAL, INPUT);
  pinMode(P1_GUARD_SIGNAL, INPUT);
  pinMode(P1_DAMAGE_SIGNAL, OUTPUT);
  pinMode(P1_SEND_LCD_PUNCH, OUTPUT);

  pinMode(P2_PUNCH_SIGNAL, INPUT);
  pinMode(P2_GUARD_SIGNAL, INPUT);
  pinMode(P2_DAMAGE_SIGNAL, OUTPUT);
  pinMode(P2_SEND_LCD_PUNCH, OUTPUT);

  pinMode(P1_END_PIN, OUTPUT);
  pinMode(P2_END_PIN, OUTPUT);

  digitalWrite(P1_SEND_LCD_PUNCH, LOW);
  digitalWrite(P2_SEND_LCD_PUNCH, LOW);
  digitalWrite(P1_END_PIN, LOW);
  digitalWrite(P2_END_PIN, LOW);

  p1_thread.onRun(p1_func);
  p2_thread.onRun(p2_func);

  controll.add(&p1_thread);
  controll.add(&p2_thread);
  
  digitalWrite(P1_END_PIN, HIGH);
  digitalWrite(P2_END_PIN, HIGH);
  digitalWrite(P1_DAMAGE_SIGNAL, HIGH);
  digitalWrite(P2_DAMAGE_SIGNAL, HIGH);
  delay(4500);
  Serial.println("Syncronize!");
  digitalWrite(P1_END_PIN, LOW);
  digitalWrite(P2_END_PIN, LOW);
  digitalWrite(P1_DAMAGE_SIGNAL, LOW);
  digitalWrite(P2_DAMAGE_SIGNAL, LOW);

  // MP3Player.volume(23);
  // MP3Player.play(2);
  // delay(4500);
  // MP3Player.volume(MUSIC_VOLUME);
  // MP3Player.loop(1);
  MP3Player.volume(MUSIC_VOLUME);
  MP3Player.play(1);
  delay(5500);
}

void p1_func() {
  static unsigned long tmp = 0;

  if (p2.status & LOOSE || p1.status & WIN) {
    p1.status = WIN;
    return;
  }
  if (p1.status & STUN) {
    if (tmp + DAMAGE_DELAY > millis())
      return;
    digitalWrite(P1_SEND_LCD_PUNCH, LOW);
    digitalWrite(P1_DAMAGE_SIGNAL, LOW);
    p1.status = 0;
  }
  if (digitalRead(P1_GUARD_SIGNAL)){ 
    p1.status = GUARDING;
    //Serial.println("p1_guarding");
  }
  else if (digitalRead(P1_PUNCH_SIGNAL)) {
    //Serial.println("p1_punching");
    p1.status = PUNCHING;
  }
  else
    p1.status = 0;
  if (!(p1.status & GUARDING) && (p2.status & PUNCHING)) {
    p1.hp -= DAMAGE;
    p1.status = STUN;
    digitalWrite(P1_SEND_LCD_PUNCH, HIGH);
    digitalWrite(P1_DAMAGE_SIGNAL, HIGH);
    Serial.println("p1_damaged");
    tmp = millis();
  }
  if (!p1.hp)
    p1.status = LOOSE;
}

void p2_func() {
  static unsigned long tmp = 0;

  if (p1.status & LOOSE || p2.status & WIN) {
    p2.status = WIN;
    return;
  }
  if (p2.status) {
    if (tmp + DAMAGE_DELAY > millis())
      return;
    digitalWrite(P2_SEND_LCD_PUNCH, LOW);
    digitalWrite(P2_DAMAGE_SIGNAL, LOW);
    p2.status = 0;
  }
  if (digitalRead(P2_GUARD_SIGNAL))
    p2.status = GUARDING;
  else if (digitalRead(P2_PUNCH_SIGNAL))
    p2.status = PUNCHING;
  else
    p2.status = 0;
  if (!(p2.status & GUARDING) && (p1.status & PUNCHING)) {
    Serial.println("p2 damaged");
    p2.hp -= DAMAGE;
    p2.status = STUN;
    digitalWrite(P2_SEND_LCD_PUNCH, HIGH);
    digitalWrite(P2_DAMAGE_SIGNAL, HIGH);
    tmp = millis();
  }
  if (!p2.hp) {
    p2.status = LOOSE;
  }

}

void reset_player() {
  p1.hp = MAX_HEALTH;
  p2.hp = MAX_HEALTH;
  p1.status = 0;
  p2.status = 0;
}

void loop() {
  controll.run();
  if (p1.status & (LOOSE | WIN) && p2.status & (LOOSE | WIN)) {
    controll.clear();
    delay(300);
    digitalWrite(P1_END_PIN, HIGH);
    digitalWrite(P2_END_PIN, HIGH);
    MP3Player.volume(20);
    if (p1.status & WIN)
      MP3Player.play(2);
    else
      MP3Player.play(3);
    while(true);
  }
}

