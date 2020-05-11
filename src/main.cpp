#include <Arduino.h>

#define DEBUG 1

// thresholds for the relay to operate in
// MOIST 450
// DRY 530
#define THRESHOLD_MOIST 450
#define THRESHOLD_DRY 530

// sum up this many moisture sensor readings to calc an averange
#define MOIST_COUNTS 2

// max time duration for watering one plant (10 seconds)
#define MAX_WATER_TIME 10000UL

// minimum time duration between watering the plants (3 days)
//#define MIN_WATER_WAIT_TIME 259200000UL
#define MIN_WATER_WAIT_TIME 10000UL

// moisture sensor on analog pin
int moist_sensor_pin_1 = A0;
int moist_sensor_pin_2 = A1;
int moist_sensor_pin_3 = A2;
int moist_sensor_pin_4 = A3;

// relay on digital pin
int relay_pin_1 = 2;
int relay_pin_2 = 3;
int relay_pin_3 = 4;
int relay_pin_4 = 5;

// magnetic switch on digital pin
int magnetic_switch_pin = 6;

// moisture calculations
int moist_val_1 = 0;
int moist_val_2 = 0;
int moist_val_3 = 0;
int moist_val_4 = 0;

int moist_avg_1 = 0;
int moist_avg_2 = 0;
int moist_avg_3 = 0;
int moist_avg_4 = 0;

int moist_val_counter = 0;
bool water_full = false;

// milliseconds the relay is on while operating
unsigned long relay_timer_on_1 = 0;
unsigned long relay_timer_on_2 = 0;
unsigned long relay_timer_on_3 = 0;
unsigned long relay_timer_on_4 = 0;

// milliseconds since the last activation of the relay
unsigned long relay_timer_off_1 = 0;
unsigned long relay_timer_off_2 = 0;
unsigned long relay_timer_off_3 = 0;
unsigned long relay_timer_off_4 = 0;

// for checking which relay is on/off
// 0001 = relay 1 is on, other are off
// 0010 = relay 2 is on, other are off
// 0100 = relay 3 is on, other are off
// 1000 = relay 4 is on, other are off
int relay_status = 0b0000;

// time calculation
unsigned long time_temp = 0;
unsigned long time_delta = 0;

void setup() {
  // init moisture sensor and relay 1
  pinMode(moist_sensor_pin_1, INPUT);
  pinMode(relay_pin_1, OUTPUT);
  digitalWrite(relay_pin_1, HIGH);

  // init moisture sensor and relay 2
  pinMode(moist_sensor_pin_2, INPUT);
  pinMode(relay_pin_2, OUTPUT);
  digitalWrite(relay_pin_2, HIGH);

  // init moisture sensor and relay 3
  pinMode(moist_sensor_pin_3, INPUT);
  pinMode(relay_pin_3, OUTPUT);
  digitalWrite(relay_pin_3, HIGH);

  // init moisture sensor and relay 4
  pinMode(moist_sensor_pin_4, INPUT);
  pinMode(relay_pin_4, OUTPUT);
  digitalWrite(relay_pin_4, HIGH);

  // init magnetic switch
  pinMode(magnetic_switch_pin, INPUT);

  time_temp = millis();

  #ifdef DEBUG
    // init Serial output
    Serial.begin(9600);
    Serial.println("Debuggin on");
  #endif
}

void water_plant(int relay_pin, int moist_avg, unsigned long relay_timer_off, unsigned long relay_timer_on, int status) {
  Serial.print("PIN: ");
  Serial.println(relay_pin - 1);
  Serial.print("moist_avg: ");
  Serial.println(moist_avg);
  Serial.print("relay_status: ");
  Serial.println(relay_status);
  Serial.print("relay_status == 0b0000: ");
  Serial.println(relay_status == 0b0000);
  Serial.print("relay_timer_off > MIN_WATER_WAIT_TIME: ");
  Serial.println(relay_timer_off > MIN_WATER_WAIT_TIME);
  if ((moist_avg > THRESHOLD_DRY)
  && (relay_status == 0b0000)
  && (relay_timer_off > MIN_WATER_WAIT_TIME)) {
    // set relay on - water the plants
    digitalWrite(relay_pin, LOW);
    // set relay status on
    relay_status |= status;
    #ifdef DEBUG
      Serial.print("relay on pin ");
      Serial.print(relay_pin - 1);
      Serial.println(" on");
      Serial.print("relay_status: ");
      Serial.println(relay_status, BIN);
    #endif

  } else if (((moist_avg < THRESHOLD_MOIST)
  || (relay_timer_on > MAX_WATER_TIME))
  && (relay_status == status)) {
    // set relay off - don't water the plants
    digitalWrite(relay_pin, HIGH);
    // set relay status off
    if ((status | relay_status) == status) {
      relay_status ^= status;
    }
    #ifdef DEBUG
      Serial.print("relay on pin ");
      Serial.print(relay_pin - 1);
      Serial.println(" off");
      Serial.print("relay_status: ");
      Serial.println(relay_status, BIN);
    #endif
  }
}

void loop() {
  water_full = !digitalRead(magnetic_switch_pin);

  if (water_full) {
    if (moist_val_counter >= MOIST_COUNTS) {
      // calculate an average
      moist_avg_1 = moist_val_1 / moist_val_counter;
      moist_avg_2 = moist_val_2 / moist_val_counter;
      moist_avg_3 = moist_val_3 / moist_val_counter;
      moist_avg_4 = moist_val_4 / moist_val_counter;

      moist_val_1 = 0;
      moist_val_2 = 0;
      moist_val_3 = 0;
      moist_val_4 = 0;

      moist_val_counter = 0;
      #ifdef DEBUG
        Serial.print("moist_val_1: ");
        Serial.println(moist_val_1);
      #endif
/*
        Serial.print("relay_timer_off_1: ");
        Serial.println(relay_timer_off_1);
        Serial.print("relay_timer_on_1: ");
        Serial.println(relay_timer_on_1);
      #endif
      */
      water_plant(relay_pin_1, moist_avg_1, relay_timer_off_1, relay_timer_on_1, 0b0001);
      water_plant(relay_pin_2, moist_avg_2, relay_timer_off_2, relay_timer_on_2, 0b0010);
      water_plant(relay_pin_3, moist_avg_3, relay_timer_off_3, relay_timer_on_3, 0b0100);
      water_plant(relay_pin_4, moist_avg_4, relay_timer_off_4, relay_timer_on_4, 0b1000);

      // time calculation
      time_delta = millis() - time_temp;
      time_temp = millis();
      
      switch (relay_status) {
        case 0b0001:
          relay_timer_on_1 += time_delta;
          relay_timer_off_1 = 0;
          relay_timer_off_2 += time_delta;
          relay_timer_off_3 += time_delta;
          relay_timer_off_4 += time_delta;
          break;

        case 0b0010:
          relay_timer_on_2 += time_delta;
          relay_timer_off_1 += time_delta;
          relay_timer_off_2 = 0;
          relay_timer_off_3 += time_delta;
          relay_timer_off_4 += time_delta;
          break;

        case 0b0100:
          relay_timer_on_3 += time_delta;
          relay_timer_off_1 += time_delta;
          relay_timer_off_2 += time_delta;
          relay_timer_off_3 = 0;
          relay_timer_off_4 += time_delta;
          break;

        case 0b1000:
          relay_timer_on_4 += time_delta;
          relay_timer_off_1 += time_delta;
          relay_timer_off_2 += time_delta;
          relay_timer_off_3 += time_delta;
          relay_timer_off_4 = 0;
          break;

        default:
          relay_timer_on_1 = 0;
          relay_timer_on_2 = 0;
          relay_timer_on_3 = 0;
          relay_timer_on_4 = 0;

          relay_timer_off_1 += time_delta;
          relay_timer_off_2 += time_delta;
          relay_timer_off_3 += time_delta;
          relay_timer_off_4 += time_delta;
      }
    } else {
      // sum up all values to calc an average
      moist_val_1 += analogRead(moist_sensor_pin_1);
      moist_val_2 += analogRead(moist_sensor_pin_2);
      moist_val_3 += analogRead(moist_sensor_pin_3);
      moist_val_4 += analogRead(moist_sensor_pin_4);

      moist_val_counter++;
    }
    // don't water the plants if the tank is empty
    digitalWrite(relay_pin_1, HIGH);
    digitalWrite(relay_pin_2, HIGH);
    digitalWrite(relay_pin_3, HIGH);
    digitalWrite(relay_pin_4, HIGH);
  }
  delay(100);
}