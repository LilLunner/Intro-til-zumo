#include <Wire.h>
#include <Zumo32U4.h>
#include <EEPROM.h>
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4IMU imu;
Zumo32U4OLED display;
#include "TurnSensor.h"

int topSpeed = 200;
int bankBalance = EEPROM.read(1);

void turnDeg(int x, int y) // x er antal rotasjoner, y er vinkeel
{
    int i = 1;
    while (i <= x) // teller antall rotasjoner
    {
        motors.setSpeeds(-100, 100);
        while ((int32_t)turnAngle < (turnAngle1 * (y))) // stopper her helt til den har truffet 90*, det virker som hva som hva som er 90 endres fra dag til dag
        {
            turnSensorUpdate();
            lineSensors.calibrate();
        }
        turnSensorReset(); // reseter gyroskop dataen
        motors.setSpeeds(0, 0);
        i++;
    }
}

void setup()
{
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    turnSensorSetup();
    turnDeg(4, 90);
    display.setLayout11x4();
}

int lineSensorRead()
{
    static unsigned int lineSensorVal[5]; // lager en variable med like mange indekser som det er sensorer
    int error = map(lineSensors.readLine(lineSensorVal), 0, 4000, -2000, 2000);
    return error;
}

void lineFollowStandard(int pos)
{
    static int lSpeed;
    static int rSpeed;
    if (pos>0){
        lSpeed = 150;
        rSpeed = 50;
    }
    else if (pos<0)
    {
        lSpeed = 50;
        rSpeed = 150;
    }
    else{
        lSpeed, rSpeed = topSpeed;
    }
    motors.setSpeeds(lSpeed, rSpeed);
}

void lineFollowPID(int pos)
{ // tar inn posisjonen
    static int prevPos;
    int correction = pos / 4 + 6 * (pos - prevPos); // kilde eksempelkode
    prevPos = pos;
    int lSpeed = topSpeed + correction;      // farten på venstre side lik topSpeed + correction
    int rSpeed = topSpeed - correction;      // farten på høgre side lik topspeed - correction
    lSpeed = constrain(lSpeed, 0, topSpeed); // setter slik at verdien vil alltids være mellom 200 og 0, vil forhindre for høye hastigheter, men viktigs
    rSpeed = constrain(rSpeed, 0, topSpeed); // hindrer at det vil fort gå fra positiv hastighet til negativ hastighet som kan skade motorene.
    motors.setSpeeds(lSpeed, rSpeed);
    if ()
}

void pressA() {
    if (buttonA.isPressed()) {
        workPickup();
    }
}
void workPickup() {
    motors.setSpeeds(0,0);
    display.print("Work starting");
    display.gotoXY(0,2);
    display.print("Picking up trash");
}

void workTransport() {
    display.print("Transporting trash.");

}

void workDropoff() {
    display.print("Dropping trash off.");
    display.gotoXY(0,2);
    display.print("Receiving 100NOK");
    bankBalance+=100;
    EEPROM.write(1, bankBalance);
}


void loop()
{
    lineFollowPID(lineSensorRead());
}