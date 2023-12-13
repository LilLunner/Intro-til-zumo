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
        if (millis() - proxTime >= 2400 / sumProx())
        {
            Serial.println("test");
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
    display.print('A');
    /*positionOfText[posArray[0]][posArray[1]] = millis();
    for (int i; i<=20; i){
        for (int j; j<=7; j){
            if (millis() - positionOfText[i][j] > 2000){
                display.gotoXY(i, j);
                display.print(" ");
                j++;
            }
            i++;
        }
    }*/
}

void loop()
{
    showProxA();
    Serial.println(sumProx());
}