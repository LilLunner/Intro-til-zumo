#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4OLED oled;
int circleMillis, zigzagMillis;

void circle() {
    motors.setSpeeds(200,100);
    if (millis()-circleMillis>=5000) {
        motors.setSpeeds(0,0);
        buttonA.waitForButton();
        circleMillis=millis(); //last 2 lines can be replaced with break in a switch case
    }
}

void zigzag() {
    if (millis()-zigzagMillis>=2300) {
        motors.setSpeeds(100,200);
    }
    else motors.setSpeeds(200,100);
    
    if (millis()-zigzagMillis>=4600) {
        zigzagMillis=millis();
        motors.setSpeeds(200,100);
    }
}

void setup() {
    buttonB.waitForButton();
    zigzagMillis=millis();
}

void loop() {
    zigzag();
}