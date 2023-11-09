#include <LiquidCrystal_I2C.h>
#include <Wire.h>
/*0x27 I2C 주소를 가지고 있는 16x2 LCD객체를 생성한다.(I2C 주소는 LCD에 맞게 수정해야 한다.)*/

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // I2C LCD를 초기화 한다.
  lcd.init();
  // I2C LCD의 백라이트를 켠다.
  lcd.backlight();
  lcd.setCursor(0,0);   
  lcd.print("■■■■■■■■■■■■■■");
  lcd.setCursor(0,1);
  lcd.print("Eng. Design");
}

void loop() { 
  // lcd.setCursor(0,0);   
  // lcd.print("■■■■■■■■■■■■■■");
  // lcd.setCursor(0,1);
  // lcd.print("Eng. Design");
}

