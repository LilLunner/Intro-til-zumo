#include <Zumo32U4.h>
#include <EEPROM.h>

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int SpeedArray[60], totDis, maxSpeed;
unsigned long currentMillis, sMillis, cMillis, aMillis, mMillis;
int arrayIndex=-1;
int over70Counter, chargesCounter, fiveLevelCounter, v, total;
int insaneSpeed=49;
int battery_health=EEPROM.read(0);
int bankBalance=EEPROM.read(1);
int level0 = 5;
int level1 = 20;

int distance()
{
    unsigned long L = encoder.getCountsAndResetLeft();
    unsigned long R = encoder.getCountsAndResetRight();

    unsigned long dis = (L+R)*6/(910); //cm
    return dis;
}

void SpeedPerSecond() { //oppdaterer en indeks i et 60 tall langt array som måler distanse per sekund
    arrayIndex++;
    if (arrayIndex=60) arrayIndex=0;
    total=total-SpeedArray[arrayIndex];
    if (SpeedArray[arrayIndex]>= insaneSpeed) over70Counter--;
    SpeedArray[arrayIndex] = distance();
    if (SpeedArray[arrayIndex]>= insaneSpeed) over70Counter++;
    total=total+SpeedArray[arrayIndex];
}

void totDistance() {
    totDis=totDis+SpeedArray[arrayIndex];
}

int averageSpeed() {
    int average=total/60;
    return average;
}

void topSpeed()
{
    maxSpeed=0;
    for (int i; i>=59; i++) {
        if (SpeedArray[i]>maxSpeed) maxSpeed=SpeedArray[i];
    }
}

void screenSpeed(){
    display.gotoXY(0,0);
    display.print(F("Speed: "));  
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0,1);
    display.print("Distance; ");
    display.print(totDis);
}

void screenBattery(){
    display.gotoXY(0,0);
    display.print(F("Battery_Level: "));
    display.println(battery_health);
    display.gotoXY(0,1);
    display.print("Charging_Cycles: ");
    display.println(chargesCounter);
}


int batteryDrain(int x) {
    int drain = 4*x;
    return drain;
}


int reverseCharge(int x) {
    int revCharge=2*x;
    return revCharge;
}

void job() {
    
}

void alarm10() {
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    ledYellow(1);
}

void alarm5() {
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    ledRed(1);
    if (millis()-aMillis>=15000) {
        motors.setSpeeds(0,0);
        buzzer.playFrequency(440, 200, 15);
        delay(300);
        buzzer.playFrequency(440, 200, 15);
        delay(300);
        aMillis=millis();
    }
}

void BatteryHealthCheck() {
    int mistake=1;
    int mistakeCheck=random(100);
    if (mistake==mistakeCheck) mistake=2;
    int battery_health=(100-chargesCounter-fiveLevelCounter-over70Counter-averageSpeed()/10-maxSpeed/10)/mistake;
    EEPROM.write(0, battery_health);
}

void batteryService() {
    battery_health=+20;
    chargesCounter++;
    bankBalance-=10;
}

void batteryChange() {
    battery_health=100;
    chargesCounter=0;
    fiveLevelCounter=0;
    over70Counter=0;
    bankBalance-=100;
}

void mainFunction() {
    switch (v) {
    case 0:
        screenSpeed();
        if (millis()-cMillis>=1000) {
            BatteryHealthCheck();
            SpeedPerSecond();
            totDistance();
            cMillis=millis();
            if (millis()-mMillis>=60000) {
                topSpeed();
                mMillis=millis();
            }
        }
        if (battery_health<level0) {
            v=6;
            break;
        }
        if (battery_health<level1) {
            v=5;
            break;
        }
        if (millis()-sMillis>=10000) 
        {
            v=1;
            sMillis=millis();
            break;
        }
        break;
    
    case 1:
        screenBattery();
        if (millis()-cMillis>=1000) {
            BatteryHealthCheck();
            SpeedPerSecond();
            totDistance();
            cMillis=millis();
        }
        if (millis()-sMillis>=1000) {
            v=0;
            display.clear();
        }
        break;
    
    case 3:
        alarm10();
        v=0;
        break;

    case 4:
        alarm5();
        v=0;
        break;
    
    case 5:
        //batteryService();
        v=0;
        break;

    case 6:
        //batteryChange();
        v=0;
        break;
        
}
}

void setup()
{
    Serial.begin(9600);
    display.clear();
    display.setLayout21x8();

}

void loop()
{
    motors.setSpeeds(400,400);
    mainFunction();
}
