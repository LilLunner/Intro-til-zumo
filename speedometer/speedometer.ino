#include <Zumo32U4.h>
#include <EEPROM.h>

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;

float SpeedArray[60], total, totDis, maxSpeed;
bool A = 1;
unsigned long currentMillis, sMillis, cMillis, aMillis, mMillis;
const int O = 12;
int arrayIndex=-1;
int screenCount=1;
int Over70Counter, chargesCounter, fiveLevelCounter, v;
int insaneSpeed=280;
int power=100;
int battery_health=EEPROM.read(0);
int level0 = 5;
int level1 = 20;

float distance()
{
    int L = encoder.getCountsAndResetLeft();
    int R = encoder.getCountsAndResetRight();

    float dis = (L+R)*6/(910); //cm
    return dis;
}

void SpeedPerSecond() {
    arrayIndex++;
    total=total-SpeedArray[arrayIndex];
    if (SpeedArray[arrayIndex]>= insaneSpeed) Over70Counter--;
    SpeedArray[arrayIndex] = distance();
    if (SpeedArray[arrayIndex]>= insaneSpeed) Over70Counter++;
    total=total+SpeedArray[arrayIndex];
}

void totDistance() {
    totDis=+SpeedArray[arrayIndex];
}

float averageSpeed(float x) {
    float average=x/60;
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
    display.clear();
    display.gotoXY(0,0);
    display.print(F("Spd: "));  
    display.println(SpeedArray[arrayIndex]);
    display.print("Dis; ");
    display.print(totDis);
}

void screenBattery(){
    display.clear();
    display.gotoXY(0,0);
    display.print(F("battery_level: "));
    //display.println(power-batteryDrain());
    display.gotoXY(0,1);
    display.print("charging_cycles: ");
    display.println(chargesCounter);
}

void screenCharge() {
    display.clear();

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

int BatteryHealthCheck() {
    int mistake=1;
    int mistakeCheck= rand() %10;
    if (mistake==mistakeCheck) mistake=2;
    int health=(100-chargesCounter-fiveLevelCounter-Over70Counter-averageSpeed(total)-topSpeed())/mistake;
    return health;
}

void batteryService() {
    battery_health=+20;
}

void batteryChange() {
    battery_health=100;
}

int main() {
    switch (v) {
    case 0:
        screenSpeed();
        if (millis()-cMillis>=1000) {
            battery_health=BatteryHealthCheck();
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
        }
    
    case 1:
        screenBattery();
        if (millis()-sMillis>=1000) {
            v=0;
        }
        break;
    
    case 2:
        screenCharge();
        v=0;
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
        batteryService();
        v=0;
        break;

    case 6:
        batteryChange();
        v=0;
        break;
        
}
}

void setup()
{
    Serial.begin(9600);
    display.clear();
    SpeedPerSecond();
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
    }
    motors.setSpeeds(400,400);
    if (millis()-cMillis>=1000) {
        SpeedPerSecond();
        cMillis=millis();
        screenSpeed();
    }*/
    //main();
}
