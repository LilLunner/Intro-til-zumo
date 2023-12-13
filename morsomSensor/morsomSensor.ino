#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4IMU imu;
Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensors;

void setup()
{
    display.init();
    display.setLayout21x8();
    proxSensors.initFrontSensor();
    display.clear();
    Serial.begin(9600);
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
        if (millis() - proxTime >= 1200 / pow(sumProx(),2))
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

void loop()
{
    showProxA();
}