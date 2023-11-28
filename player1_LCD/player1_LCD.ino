#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <LCDGraph.h>

#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

#define LED_RED 2
#define LED_BLUE 3
#define LEF_WHITE 4

ThreadController controll = ThreadController();

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
LCDGraph<float, LiquidCrystal_I2C> graph(8, 0); // We want to store floats in this case

void setup() {
  Serial.begin(9600);

  // Set up the lcd
  lcd.init();
  lcd.backlight();
  graph.begin(&lcd);

  // Draw the graph:
  graph.yMin = 0;
  graph.yMax = 1;
  graph.filled = true;
  graph.setRegisters();
  graph.display(0,1);
  for (int i = 0; i < 8 * 5; i++) {
    graph.add(1);
  }
  graph.setRegisters();

  pinMode(A3, INPUT);
  //pinMode(LED_RED, OUTPUT);
  //pinMode(LED_BLUE, OUTPUT);
  //pinMode(LED_WHITE, OUTPUT);
}

void loop() {
  while (!digitalRead(A3))
    delay(10);
  for (int i = 0; i < 5; i++) {
      graph.add(0);
      graph.setRegisters();
  }
  delay(600);
}