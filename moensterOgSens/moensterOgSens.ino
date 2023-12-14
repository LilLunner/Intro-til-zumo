#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4OLED display;
Zumo32U4IMU imu;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

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

void circle()
{
    motors.setSpeeds(200, 100);
    if (millis() - circleMillis >= 5000)
    {
        motors.setSpeeds(0, 0);
        buttonA.waitForButton();
        circleMillis = millis(); // last 2 lines can be replaced with break in a switch case
    }
}

void zigzag()
{
    if (millis() - zigzagMillis >= 2300)
    {
        motors.setSpeeds(100, 200);
    }
    else
        motors.setSpeeds(200, 100);

    if (millis() - zigzagMillis >= 4600)
    {
        zigzagMillis = millis();
        motors.setSpeeds(200, 100);
    }
}

int sumProx()
{
    proxSensors.read();
    int sumProxa = proxSensors.countsFrontWithLeftLeds() + proxSensors.countsFrontWithRightLeds();
    return sumProxa;
}

void rnd(int pposArray[])
{
    static unsigned long proxTime;
    if (sumProx() != 0)
    {
        if (millis() - proxTime >= 1200 / pow(sumProx(), 2))
        {
            pposArray[0] = random(0, 20);
            pposArray[1] = random(0, 7);
            proxTime = millis();
        }
    }
    else
    {
        pposArray[0] = 27;
        pposArray[1] = 27;
    }
}

void showProxA()
{
    static unsigned long positionOfText[21][8];
    static int posArray[2];
    rnd(posArray);
    display.gotoXY(posArray[0], posArray[1]);
    display.print('O');
    positionOfText[posArray[0]][posArray[1]] = millis();

    for (int i = 0; i <= 20; i)
    {
        for (int j = 0; j <= 7; j)
        {
            if ((positionOfText[i][j] != 0) && (millis() - positionOfText[i][j] > random(500, 4000)))
            {
                display.gotoXY(i, j);
                display.print(" ");
                positionOfText[i][j] = 0;
            }
            j++;
        }
        i++;
    }
}

void pressA() {
    if (buttonA.isPressed())
        square();
}

void pressB() {
    if (buttonB.isPressed())
        circle();
}

void pressC() {
    if (buttonC.isPressed())
        zigzag();
}

void Interface() {
    display.print("Press A for square.");
    display.gotoXY(0,2);
    display.print("Press B for circle.");
    display.gotoXY(0,4);
    display.print("Press C for slalom.");
}

void setup()
{
    turnSensorSetup();
    zigzagMillis = millis();
    display.init();
    display.setLayout21x8();
    proxSensors.initFrontSensor();
    display.clear();
}

void loop()
{
    square();
    showProxA();
}