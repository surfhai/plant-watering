#include "Ultrasonic.h"

int ultrasonicPin = 3;

Ultrasonic ultrasonic(ultrasonicPin);
void setup()
{
    Serial.begin(9600);
}
void loop()
{
    long RangeInCentimeters;

    RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
    Serial.print(RangeInCentimeters);//0~400cm
    Serial.println(" cm");
    delay(250);
}
