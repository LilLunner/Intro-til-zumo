#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4OLED display;
Zumo32U4IMU imu;

#include "TurnSensor.h"

int circleMillis, zigzagMillis;

void turnDeg(int x, int y) // x er antal rotasjoner, y er vinkeel
{
    int i = 1;
    while (i <= x) // teller antall rotasjoner
    {
        motors.setSpeeds(-100, 100);
        while ((int32_t)turnAngle < (turnAngle1 * (y))) // stopper her helt til den har truffet 90*, det virker som hva som hva som er 90 endres fra dag til dag
        {
            turnSensorUpdate();
        }
        turnSensorReset(); // reseter gyroskop dataen
        motors.setSpeeds(0, 0);
        i++;
    }
}

void square()
{
    unsigned long time = millis();
    static int check, prevCheck;
    for (int i = 0; i <= 4; i)
    {
        motors.setSpeeds(150, 150);
        if ((millis() - 2500) > time)
        {
            motors.setSpeeds(0, 0);
            delay(100);
            turnDeg(1, 87);
            time = millis();
            i++;
        }
    }
}

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
    turnSensorSetup();
    zigzagMillis=millis();
}

void loop() {
    square();
    delay(500000);
}