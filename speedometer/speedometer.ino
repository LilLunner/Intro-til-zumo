#include <Zumo32U4.h>
#include <Zumo32U4Encoders.h> 

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;

int SpeedArray[60];
float totdis;
bool A = 1;
unsigned long currentMillis, sMillis;
const int O = 12;
int arrayIndex=-1;
int screenCount=1;
int maxSpeed, total, Over70Counter;
int insaneSpeed=280;

int distance()
{
    int L = encoder.getCountsAndResetLeft();
    int R = encoder.getCountsAndResetRight();

    int dis = (L+R)/2;///(12*75);
    return dis;
}

int totalDistance()
{
    totdis=totdis+SpeedArray[arrayIndex];
    return totdis;
}

void SpeedPerSecond() {
    arrayIndex++;
    total=total-SpeedArray[arrayIndex];
    if (SpeedArray[arrayIndex]>= insaneSpeed) Over70Counter--;
    SpeedArray[arrayIndex] = distance();
    if (SpeedArray[arrayIndex]>= insaneSpeed) Over70Counter++;
    total=total+SpeedArray[arrayIndex];
}

int averageSpeed(int x) {
    int average=x/60;
    return average;
}

int topSpeed(int x)
{
 if (maxSpeed<x) maxSpeed=x;
 return maxSpeed;
}

void screen1(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Spd: "));  
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0,1);
    display.print("Dis; ");
    display.print(totalDistance());
}

void screen2(){
    /*display.clear();
    display.gotoXY(0,0);
    display.print(F("Distance: "));
    display.println(toDistance(distance()));*/
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
    /*if (millis()-currentMillis>=1000)
    {
    SpeedPerSecond();
    motors.setSpeeds(400,400);
    screen1();
    currentMillis=millis();
    if (arrayIndex>=59) arrayIndex=-1;
    }*/
}
