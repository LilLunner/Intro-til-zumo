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
int deg90 = 90;
int v;

#include "TurnSensor.h"

void turnDeg(int x, int y) // x er antal rotasjoner, y er vinkeel
{
    int i = 1;
    while (i <= x) // teller antall rotasjoner
    {
        motors.setSpeeds(-100, 100);
        while ((int32_t)turnAngle < (turnAngle1 * (y))) // stopper her helt til den har truffet 90*, det virker som hva som hva som er 90 endres fra dag til dag
        {
            turnSensorUpdate();
            showProxA();
        }
        turnSensorReset(); // reseter gyroskop dataen
        motors.setSpeeds(0, 0);
        i++;
    }
}

void square()
{
    uint32_t time = millis();
    static int check, prevCheck;
    for (int i = 0; i <= 4; i)
    {
        motors.setSpeeds(150, 150);
        showProxA();
        if ((millis() - 2500) > time)
        {
            motors.setSpeeds(0, 0);
            turnDeg(1, deg90);
            time = millis();
            i++;
        }
    }
}

void circle()
{
    uint32_t circleMillis = millis();
    motors.setSpeeds(200, 100);
    while (millis() - circleMillis < 6000)
        showProxA();
}

void zigzag()
{
    bool x = 0;
    uint32_t zigzagMillis = millis();
    for (int i = 0; i <= 3; i)
    {
        turnSensorUpdate();
        showProxA();
        if (x == 0)
        {
            motors.setSpeeds(200, 100);
            if ((int32_t) turnAngle >= turnAngle1*deg90) 
            {
                i++;
                zigzagMillis = millis();
                x = !x;
                turnSensorReset();  
            }
        }
        else
        {
            motors.setSpeeds(100, 200);
           if ((int32_t) turnAngle >= turnAngle1*deg90) 
            {
                i++;
                zigzagMillis = millis();
                x = !x;
                turnSensorReset();  
            }
        }
        
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
    static uint32_t proxTime;
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
    static uint32_t positionOfText[21][8];
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

void pressA()
{
    if (buttonA.isPressed())
        v = 1;
}

void pressB()
{
    if (buttonB.isPressed())
        v = 2;
}

void pressC()
{
    if (buttonC.isPressed())
        v = 3;
}

void Interface()
{
    display.print("Press A for square.");
    display.gotoXY(0, 2);
    display.print("Press B for circle.");
    display.gotoXY(0, 4);
    display.print("Press C for slalom.");
}

void patterns()
{
    pressA();
    pressB();
    pressC();
    switch (v)
    {
    case 0:
        motors.setSpeeds(0, 0);
        break;
    case 1:
        square();
        v = 0;
        break;
    case 2:
        circle();
        v = 0;
        break;
    case 3:
        zigzag();
        v = 0;
        break;
    }
}

void setup()
{
    turnSensorSetup();
    display.init();
    display.setLayout21x8();
    proxSensors.initFrontSensor();
    display.clear();
    Interface();
}

void loop()
{
    patterns();
    showProxA();
}