#include <Zumo32U4.h>
#include <Zumo32U4Encoders.h>

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;

float q, i, totDiss;
bool A = 1;
unsigned long currentMillis, sMillis, t;
const int speedCheck = 250;
const int O = 12;
int screenCount = 1;

float distance()
{
    float L = encoder.getCountsLeft();
    float R = encoder.getCountsRight();
    if (A == 1)
    {
        i = sqrt(L * L + R * R);
        A = 0;
        t = millis();
    }
    if ((millis() - t) >= speedCheck)
    {
        q = sqrt(L * L + R * R);
        A = 1;
    }
    float dis = abs(((i - q) / (910)) * O);
    return dis;
}

float toDistance(float y)
{
    totDiss = (totDiss + y)/100; //totDiss + (y/100)
    return totDiss;
}


float speed(float x)
{
    float hastighet = x * (1000/speedCheck);
    return hastighet;
}
void screen1(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Speed:"));
    display.gotoXY(0,1);
    display.print(speed(distance()));
}

void screen2(){
    display.clear();
    display.gotoXY(0,0);
    display.println(F("Distance:"));
    display.gotoXY(0,1);
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
    motors.setSpeeds(-50, 100);
    distance();
    if ((millis() - currentMillis) >= speedCheck)
    {
        float d = distance();
        //Serial.println(d);
        currentMillis = millis();
        Serial.println(speed(distance()));
        Serial.print("totaldiss");
        Serial.println(toDistance(d));
    }
}