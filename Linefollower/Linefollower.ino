#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4IMU imu;
Zumo32U4OLED display;
#include "TurnSensor.h"

int topSpeed = 200;

void turnDeg(int x, int y) // x er antal rotasjoner, y er vinkeel
{
    int i = 1;
    while (i <= x) // ekke helt sikker på hvofor denne  er nødvendig, men funker ikke uten
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

void lineCalibrate()
{
    turnDeg(4, 90);
    lineSensors.calibrate();
}

void setup()
{
    turnSensorSetup();
    lineCalibrate();
}

int lineSensorRead()
{
    static int prevError;
    static unsigned int lineSensorVal[5];
    int error = map(lineSensors.readLine(lineSensorVal), 0, 4000, -2000, 2000);
    int correction = error/4+6*(error-prevError); //kilde eksempelkode
    prevError = error;
    return correction;
}

void lineFollow(int correction){
    int lSpeed = topSpeed + correction;
    int rSpeed = topSpeed - correction;
    lSpeed = constrain(lSpeed, 0, topSpeed);
    rSpeed = constrain(rSpeed, 0, topSpeed);
    motors.setSpeeds(lSpeed, rSpeed);
}

void loop()
{
    lineFollow(lineSensorRead());
    //pasdlhiafhaeughå
}