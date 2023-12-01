#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <LCDGraph.h>

#define PLAYER "1P"

#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

#define LED_BLUE 9
#define LED_WHITE 10
#define LED_RED 11
#define END_GAME_PIN 12

ThreadController damage_funcs = ThreadController();
ThreadController game_status = ThreadController();


LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
LCDGraph<float, LiquidCrystal_I2C> graph(8, 0); // We want to store floats in this case

Thread lcd_thread = Thread();
Thread led_thread = Thread();
Thread game_status_thread = Thread();

int health_cnt;
bool game_end;

void setup() {
  Serial.begin(9600);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  pinMode(END_GAME_PIN, INPUT);

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_WHITE, LOW);

  game_end = false;

  // Set up the lcd
  lcd.init();
  lcd.backlight();
  graph.begin(&lcd);

  led_thread.onRun(led_func);
  lcd_thread.onRun(lcd_func);
  game_status_thread.onRun(game_status_func);
  game_status_thread.setInterval(10);

  damage_funcs.add(&lcd_thread);
  damage_funcs.add(&led_thread);
  game_status.add(&game_status_thread);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Loading....");
  delay(2000);
  while(digitalRead(END_GAME_PIN));
  Serial.println("Syncronize:)");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO HFC!");
  delay(5500);
  health_bar_reset();

  pinMode(A3, INPUT);
  digitalWrite(LED_WHITE, HIGH);

}

void health_bar_reset() {
  health_cnt = 8;

  lcd.clear();

  for (int i = 3; i > 0; i--) {
    lcd.setCursor(5, 0);
    lcd.print(i);
    delay(300);
  }
  lcd.setCursor(2,0);
  lcd.print("FIGHT!!");
  delay(500);
  lcd.clear();

  lcd.setCursor(5,0);
  lcd.print(PLAYER);
  // Draw the graph:
  graph.yMin = 0;
  graph.yMax = 1;
  graph.filled = true;
  graph.setRegisters();
  graph.display(0,1);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 5; j++) 
      graph.add(1);
    graph.setRegisters();
  }
  graph.setRegisters();
}

void lcd_func() {
  for (int i = 0; i < 5; i++) {
      graph.add(0);
      if (i % 2 == 0)
        graph.setRegisters();
  }
}

void led_func() {
  unsigned long tmp;

  for (int i = 0; i < 3; i++) {
    tmp = millis();
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_WHITE, LOW);
    while (tmp + 100 > millis());
    tmp = millis();
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_WHITE, HIGH);
    while (tmp + 100 > millis());
  }
}

void winner() {
  lcd.clear();
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(3, 0);
    lcd.print("WINNER!!");
    delay(800);
    lcd.clear();
    delay(200);
  }
}

void looser() {
  for (int i = 0; i < 3; i++) {
    lcd.clear();
    lcd.setCursor(2 + i, 0);
    lcd.print("LOOSER..");
    delay(333);
  }
  for (int i = 2; i >= 0; i--) {
    lcd.clear();
    lcd.setCursor(2 + i, 0);
    lcd.print("LOOSER..");
    delay(333);
  }
  for (int i = 0; i < 3; i++) {
    lcd.clear();
    lcd.setCursor(2 + i, 0);
    lcd.print("LOOSER..");
    delay(333);
  }
  for (int i = 2; i >= 0; i--) {
    lcd.clear();
    lcd.setCursor(2 + i, 0);
    lcd.print("LOOSER..");
    delay(333);
  }
}

void game_status_func() {
  static unsigned long tmp = 0;

  if (digitalRead(END_GAME_PIN)) {
    game_status.clear();
    damage_funcs.run();
    game_end = true;
  }
  else if (digitalRead(A3) && tmp + 500 < millis()) {
    damage_funcs.run();
    health_cnt--;
    tmp = millis();
  }
}

void loop() {
  game_status.run();
  if (game_end) {
    if (health_cnt) {
      while (true) winner();
    }
    else {
      while (true) looser();
    }
  }
}