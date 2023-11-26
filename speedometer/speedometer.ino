#include <Zumo32U4.h>
#include <Zumo32U4Encoders.h>
#include <movingAvg.h>  

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;

float SpeedArray[60];
float q, i, totdis;
bool A = 1;
unsigned long currentMillis, sMillis, t;
const int speedCheck = 250;
const int O = 12;
int arrayIndex;

float distance()
{
    int L = encoder.getCountsAndResetLeft();
    int R = encoder.getCountsAndResetRight()

    float dis = (L+R)/(12*75)*;
    return dis;
}

float totalDistance(distance) {
    totdis=totdis+distance;
    return totdis;
}

void SpeedPerMinute() {
    SpeedArray[arrayIndex] = distance();
    arrayIndex++;
    if (arrayIndex=>60) arrayIndex=0;
}

float averageSpeed(SpeedArray) {
    float total;
    for (int i; i<=59; i++) {
        total=total+SpeedArray[i];
    }
    float average=total/60;
    return average;
}


float speed(float x)
{
    float hastighet = x * (1000/speedCheck);
    return hastighet;
}
void screen1(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Speed: "));  
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0,1);
    display.print("Distance; ");
    display.print(totalDistance(distance()));
}

void screen2(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Distance: "));
    display.println(toDistance(distance()));
}

void Oled() {
    const int screenTime = 5000;
    if ((millis()-sMillis)>= screenTime){
        if (screenCount == 1){
            screen1();
        }
        else {
            screen2();
            screenCount = 0;
        }
        sMillis=millis();
        screenCount++;
    }
}

void setup()
{
    Serial.begin(9600);
    display.clear();
}

void loop()
{
    Oled();
    motors.setSpeeds(100,100);
}
