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
unsigned long sMillis, cMillis, aMillis, mMillis, lMillis;
int arrayIndex=-1;
int over70Counter, chargesCounter, fiveLevelCounter, v, total;
int battery_health=EEPROM.read(0);
int bankBalance=EEPROM.read(1);

int distance()
{
    long int L = encoder.getCountsAndResetLeft();
    long int R = encoder.getCountsAndResetRight();

    long int dis = (L+R)*6/(910); //cm
    return dis;
}

void SpeedPerSecond() { //oppdaterer en indeks i et 60 tall langt array som måler distanse per sekund
    arrayIndex++;
    if (arrayIndex=60) arrayIndex=0;
    total=total-SpeedArray[arrayIndex];
    if (SpeedArray[arrayIndex]>= 49) over70Counter--;
    SpeedArray[arrayIndex] = distance();
    if (SpeedArray[arrayIndex]>= 49) over70Counter++;
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

void buttonDisplay() {
    display.gotoXY(0,3);
    display.print("Press A to charge");
    display.gotoXY(0,4);
    display.print("Costs 10NOK");
    display.gotoXY(0,6);
    display.print("Press B to replace");
    display.gotoXY(0,7);
    display.print("Costs 100NOK");
}

void screenSpeed(){
    display.gotoXY(0,0);
    display.print(F("Speed: "));  
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0,1);
    display.print("Distance; ");
    display.print(totDis);
    buttonDisplay();
}

void screenBattery(){
    display.gotoXY(0,0);
    display.print("Battery_Level: ");
    display.println(battery_health);
    display.gotoXY(0,1);
    display.print("Charging_Cycles: ");
    display.println(chargesCounter);
    buttonDisplay();
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
    buttonDisplay();
    ledYellow(1);
    delay(300);
    buzzer.stopPlaying();
}

void alarm5() {
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.println("Battery health");
    display.print(battery_health);
    display.gotoXY(0,1);
    display.println("Battery low!");
    buttonDisplay();
    ledRed(1);
    motors.setSpeeds(0,0);
    buzzer.playFrequency(440, 200, 15);
    delay(1000);
    buzzer.stopPlaying();
    aMillis=millis();
}

void emptyBattery() {
    motors.setSpeeds(0,0);
    display.print("Battery empty");
    display.gotoXY(0,1);
    display.print("Please change battery");
}

void BatteryHealthCheck() {
    int mistake=1;
    int mistakeCheck=random(100);
    if (mistake==mistakeCheck) mistake=2;
    battery_health=(battery_health-chargesCounter-fiveLevelCounter-over70Counter-averageSpeed()/10-maxSpeed/10)/mistake;
    EEPROM.write(0, battery_health);
}

void batteryService() {
    motors.setSpeeds(0,0);
    battery_health+=10;
    chargesCounter++;
    bankBalance-=10;
    display.clear();
    display.println("Battery charged");
    display.println(battery_health);
    display.print("%");
    display.gotoXY(0,2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(2000);
    display.clear();
}

void pressA() {
    if (buttonA.isPressed()) {
        v=5;
    }
}

void pressB() {
    if (buttonB.isPressed()) {
        v=6;
    }
}

void batteryChange() {
    motors.setSpeeds(0,0);
    battery_health=100;
    chargesCounter=0;
    fiveLevelCounter=0;
    over70Counter=0;
    bankBalance-=100;
    display.clear();
    display.print("Battery changed!");
    display.gotoXY(0,2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(2000);
    display.clear();
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

        if (battery_health<=0) {
            v=7;
            break;
        }
        if (battery_health<5) {
            v=4;
            break;
        }
        if (battery_health<20) {
            v=3;
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
        if (millis()-lMillis>=15000) {
        alarm10();
        lMillis=millis();
        }
        v=0;
        break;

    case 4:
        if (millis()-sMillis>=15000) {
        alarm5();
        lMillis=millis();
        }
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
    case 7:
        emptyBattery();
        if (battery_health>0) {
            v=0;
        }       
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
    static int battery_health=5;
    motors.setSpeeds(400,400);
    pressA();
    pressB();
    mainFunction();
}
