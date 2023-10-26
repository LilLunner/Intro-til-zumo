#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4OLED display;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];


void setup()
{
    calibrateSensors();
}

void calibrateSensors()
{
    // display.clear();

    // Wait 1 second and then begin automatic sensor calibration
    // by rotating in place to sweep the sensors over the line
    delay(1000);
    for (uint16_t i = 0; i < 120; i++)
    {
        motors.setSpeeds(-200, 200);
        lineSensors.calibrate();
    }
    motors.setSpeeds(0, 0);
}

int position()
{
    int val;
    val = map(lineSensors.readLine(lineSensorValues), 0, 4000, -200, 200); 
    return val;
}

void drive(int x)
{
   const int defaultSpeed = 200;
   int leftSpeed = defaultSpeed + x;
   int rightSpeed = defaultSpeed - x;
   motors.setSpeeds(leftSpeed, rightSpeed);
}
void loop()
{
    drive(position());
}