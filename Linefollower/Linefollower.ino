#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;

const uint16_t maxSpeed = 400;
int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

void calibrate() {

    lineSensors.calibrate();
}

void lineDrive() {
    int position = lineSensors.readLine(lineSensorValues);
    int error = position-2000;
    int speedDifference = error / 4 + 6 * (error - lastError);
    lastError = error;
    int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
    int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

    leftSpeed = constrain(leftSpeed, 0, (int16_t)maxSpeed);
    rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);

}

void setup() {
    lineSensors.initFiveSensors();
}

void loop() {

}