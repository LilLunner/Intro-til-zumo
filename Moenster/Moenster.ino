#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA button;
Zumo32U4OLED oled;
int circleMillis;

void circle() {
    motors.setSpeeds(200,100);
    if (millis()-circleMillis>=5000) {
        motors.setSpeeds(0,0);
        button.waitForButton();
        circleMillis=millis(); //last 2 lines can be replaced with break in a switch case
    }
}

void setup() {
    button.waitForButton();
    circleMillis=millis();
}

void loop() {
    circle();
}