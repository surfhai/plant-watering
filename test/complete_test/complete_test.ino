#include <Wire.h>
#include "SSD1803a_I2C.h"
#include "RTClib.h"
#include "Ultrasonic.h"

// thresholds for the relay to operate in
// MOIST 450
// DRY 530
int moisture_threshold_moist = 450;
int moisture_threshold_dry = 530;

// sum up this many moisture sensor readings to calc an averange
#define MOIST_COUNTS 10

// max time duration for watering one plant (10 seconds)
unsigned long watering_duration = 10000;

// minimum time duration between watering the plants (3 days)
//#define MIN_WATER_WAIT_TIME 259200000UL
//#define MIN_WATER_WAIT_TIME 10000UL
unsigned long watering_pause = 259200000;

// minimum water level for watering
byte watering_min_level = 8;

enum Menu {
    STATUS,
    MOISTURE_THRESHOLD_MOIST,
    MOISTURE_THRESHOLD_DRY,
    WATERING_DURATION,
    WATERING_PAUSE,
    WATERING_MIN_LEVEL,
    DATE_TIME
};
Menu menu = Menu::STATUS;

#define MENU_LAST Menu::DATE_TIME
#define MENU_LAST_INDEX 6

const byte i2c_addr  = 0x3c;
const byte reset_pin = 12;

const byte btn_up = 4;
const byte btn_down = 5;
const byte btn_sel = 6;

const byte moist_pin = A0;
const byte relay_pin = 2;
const byte ultrasonic_pin = 3;

int moist_val = 0;
int water_level = 0;
long int last_update = 0;
long int last_menu_update = 0;
bool edit_menu = 0;


SSD1803a_I2C lcd(i2c_addr,reset_pin);
Ultrasonic us(ultrasonic_pin);
RTC_DS1307 rtc;
DateTime now;

void setup() {
    Serial.begin(9600);
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }
    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    pinMode(btn_up, INPUT);
    pinMode(btn_down, INPUT);
    pinMode(btn_sel, INPUT);
    pinMode(moist_pin, INPUT);
    pinMode(relay_pin, OUTPUT);

    // 20 characters and 4 lines
    lcd.begin(20,4);
    lcd.setContrast(0x38);
    
    last_update = rtc.now().unixtime();
    last_menu_update = rtc.now().unixtime();
}

void loop() {
    now = rtc.now();

    now.timestamp(DateTime::TIMESTAMP_TIME);

    moist_val = 0;
    
    for (int i = 0; i < MOIST_COUNTS; ++i) {
        moist_val += analogRead(moist_pin);
    }

    moist_val = moist_val / MOIST_COUNTS;
    water_level = us.MeasureInCentimeters();

    if (now.unixtime() - last_update > 60) {
        Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME));
        Serial.print(" Moisture ");
        Serial.print(moist_val);
        Serial.print("\tWater Level: ");
        Serial.print(water_level);
        Serial.print("cm\n");

        last_update = now.unixtime();
    }

    if (now.unixtime() - last_menu_update > 5) {
        updateMenu();
        delay(100);
    }

    if (digitalRead(btn_sel) == LOW) {
        if (edit_menu) {
            edit_menu = 0;
        } else {
            edit_menu = 1;
        }
        updateMenu();
        delay(100);
        while (digitalRead(btn_sel) == LOW);;
    }
    
    if (digitalRead(btn_up) == LOW) {
        if (edit_menu) {
            switch (menu){
                case Menu::MOISTURE_THRESHOLD_MOIST:
                    moisture_threshold_moist += 5;
                    break;
                case Menu::MOISTURE_THRESHOLD_DRY:
                    moisture_threshold_dry += 5;
                    break;
                case Menu::WATERING_DURATION:
                    watering_duration += 500;
                    break;
                case Menu::WATERING_PAUSE:
                    watering_pause += 3600000;
                    break;
                case Menu::WATERING_MIN_LEVEL:
                    watering_min_level += 1;
                    break;
            }
        } else {
            if (int(menu) == 0) {
                menu = MENU_LAST;
            } else {
                menu = Menu(int(menu) - 1);
            }
        }
        updateMenu();
        delay(100);
        while (digitalRead(btn_up) == LOW);
    }
    
    if (digitalRead(btn_down) == LOW) {
        if (edit_menu) {
            switch (menu){
                case MOISTURE_THRESHOLD_MOIST:
                    moisture_threshold_moist -= 5;
                    break;
                case MOISTURE_THRESHOLD_DRY:
                    moisture_threshold_dry -= 5;
                case WATERING_DURATION:
                    watering_duration -= 500;
                case WATERING_PAUSE:
                    watering_pause -= 3600000;
                    break;
                case Menu::WATERING_MIN_LEVEL:
                    watering_min_level -= 1;
                    break;
            }
        } else {
            //
            if (int(menu) == MENU_LAST_INDEX) {
                menu = Menu::STATUS;
            } else {
                menu = Menu(int(menu) + 1);
            }
        }
        updateMenu();
        delay(100);
        while (digitalRead(btn_down) == LOW);
    }
}

void updateMenu() {
    switch (menu) {
        case Menu::STATUS:
            menuDefault();
            break;
        case Menu::MOISTURE_THRESHOLD_MOIST:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Moisture Threshold");
            lcd.setCursor(0, 1);
            lcd.print(">Moist: ");
            lcd.print(moisture_threshold_moist);
            lcd.setCursor(0, 2);
            lcd.print(" Dry: ");
            lcd.print(moisture_threshold_dry);
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        case Menu::MOISTURE_THRESHOLD_DRY:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Moisture Threshold");
            lcd.setCursor(0, 1);
            lcd.print(" Moist: ");
            lcd.print(moisture_threshold_moist);
            lcd.setCursor(0, 2);
            lcd.print(">Dry: ");
            lcd.print(moisture_threshold_dry);
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        case Menu::WATERING_DURATION:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Watering Duration");
            lcd.setCursor(0, 1);
            lcd.print(watering_duration / 1000);
            lcd.print("s");
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        case Menu::WATERING_PAUSE:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Watering Pause");
            lcd.setCursor(0, 1);
            lcd.print(watering_pause / 1000 / 60 / 60 / 24);
            lcd.print("d ");
            lcd.print(watering_pause / 1000 / 60 / 60 % 24);
            lcd.print("h");
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        case Menu::WATERING_MIN_LEVEL:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Min. Watering Level");
            lcd.setCursor(0, 1);
            lcd.print(watering_min_level);
            lcd.print("cm");
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        case Menu::DATE_TIME:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("TBD");
            lcd.setCursor(0, 1);
            lcd.print("");
            
            if (edit_menu) {
                lcd.setCursor(0, 3);
                lcd.print("Change Value");
            } else {
                lcd.setCursor(0, 3);
                lcd.print("                    ");
            }
            break;
        default:
            menu = Menu::STATUS;
            menuDefault();
            break;
    }
    last_menu_update = now.unixtime();
}

void menuDefault() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(now.timestamp(DateTime::TIMESTAMP_FULL));
    lcd.setCursor(0, 1);
    lcd.print("Moisture: ");
    lcd.print(moist_val);
    lcd.setCursor(0, 2);
    lcd.print("Water Level: ");
    lcd.print(water_level);
    lcd.print("cm");
}
