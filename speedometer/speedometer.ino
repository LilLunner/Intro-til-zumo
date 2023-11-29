#include <Zumo32U4.h>
#include <EEPROM.h>

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;

int SpeedArray[60];
float totdis;
bool A = 1;
unsigned long currentMillis, sMillis;
const int O = 12;
int arrayIndex=-1;
int screenCount=1;
int maxSpeed, total, Over70Counter, charges;
int insaneSpeed=280;
int power=100;

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

void screenSpeed(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Spd: "));  
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0,1);
    display.print("Dis; ");
    display.print(totalDistance());
}

void screenBattery(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("battery_level: "));
    //display.println(power-batteryDrain());
    display.gotoXY(0,1);
    display.print("charging_cycles: ");
    display.println(charges);
}

void screenCharge() {
    display.clear();

}


void Oled() {
    const int screenTime = 10000;
    if ((millis()-sMillis)>= 5000){
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

int batteryDrain(int x) {
    int drain = 4*x;
    return drain;
}


int reverseCharge(int x) {
    int revCharge=2*x;
    return revCharge;
}

int emergencyCharge(int x) {
        int batteryGain=10*x;
        return batteryGain;
}

void alarm10() {
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    ledYellow(1);
    delay(1000);
    ledYellow(0);
    delay(1000);
}

void alarm5() {
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    ledRed(1);
    delay(1000);
    ledRed(0);
    delay(1000);
}

int BatteryHealth() {
    
}

void setup()
{
    Serial.begin(9600);
    display.clear();
    battery_health=EEprom.read(0);
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
    alarm10();
}
