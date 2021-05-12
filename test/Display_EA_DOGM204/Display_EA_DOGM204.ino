// https://github.com/firexx/SSD1803a_I2C

#include "SSD1803a_I2C.h"

const byte i2c_addr  = 0x3c;
const byte reset_pin = 12;

const byte btn_up = 4;
const byte btn_down = 5;
const byte btn_sel = 6;

SSD1803a_I2C lcd(i2c_addr,reset_pin);

void setup() {  
    pinMode(btn_up, INPUT);
    pinMode(btn_down, INPUT);
    pinMode(btn_sel, INPUT);

    // 20 characters and 4 lines
    lcd.begin(20,4);
    lcd.setContrast(0x38);
}

void loop() {
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("hello world");
    lcd.setCursor(0, 1);
    lcd.println("hello world");
    lcd.setCursor(0, 2);
    lcd.println("hello world");
    lcd.setCursor(0, 3);
    lcd.println("hello world");

    if (digitalRead(btn_up) == LOW) {
        lcd.clear();
        delay(1000);
    }
    /*
    lcd.setCursor(0, 1);
    if (digitalRead(btn_up) == LOW) {
        lcd.println("Button Up: pressed");
    } else {
        lcd.println("Button Up: not press");
    }
    
    lcd.setCursor(0, 2);
    if (digitalRead(btn_down) == LOW) {
        lcd.println("Button Down: pressed");
    } else {
        lcd.println("Button Down: not press");
    }

    lcd.setCursor(0, 3);
    if (digitalRead(btn_sel) == LOW) {
        lcd.println("Button Sel: pressed");
    } else {
        lcd.println("Button Sel: not press");
    }
    */
}
