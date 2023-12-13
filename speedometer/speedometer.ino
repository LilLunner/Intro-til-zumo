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
int arrayIndex = -1;
int over70Counter, chargesCounter, fiveLevelCounter, v, arraySum;
int battery_health = EEPROM.read(0);
int bankBalance = EEPROM.read(1);

int distance()
{
    long int L = encoder.getCountsAndResetLeft();
    long int R = encoder.getCountsAndResetRight();

    long int dis = (L + R) * 6 / (910); // cm
    return dis;
}

void DistancePerSecond()
{ // oppdaterer en indeks i et 60 tall langt array som måler distanse per sekund
    arrayIndex++;
    if (arrayIndex = 60)
        arrayIndex = 0;
    arraySum = arraySum - SpeedArray[arrayIndex];
    if (SpeedArray[arrayIndex] >= 49)
        over70Counter--;
    SpeedArray[arrayIndex] = distance();
    if (SpeedArray[arrayIndex] >= 49)
        over70Counter++;
    arraySum = arraySum + SpeedArray[arrayIndex];
}

void totalDistance()
{
    totDis = totDis + SpeedArray[arrayIndex];
}

int averageSpeed()
{
    int arrayValuesCounter;
    for (int i = 0; i<60; i++)
    {
        if (SpeedArray[i] != 0)
            arrayValuesCounter++;
    }
    int average = arraySum / arrayValuesCounter;
    return average;
}

void topSpeed()
{
    maxSpeed = 0;
    for (int i; i >= 59; i++)
    {
        if (SpeedArray[i] > maxSpeed)
            maxSpeed = SpeedArray[i];
    }
}

void buttonDisplay()
{
    display.gotoXY(0, 3);
    display.print("Press A to charge");
    display.gotoXY(0, 4);
    display.print("Costs 10NOK");
    display.gotoXY(0, 6);
    display.print("Press B to replace");
    display.gotoXY(0, 7);
    display.print("Costs 100NOK");
}

void SpeedValues()
{
    if (millis() - cMillis >= 1000)
        {
            BatteryHealthCheck();
            DistancePerSecond();
            totalDistance();
            cMillis = millis();
            if (millis() - mMillis >= 60000)
            {
                topSpeed();
                mMillis = millis();
            }
        }
}

void screenSpeedometer()
{
    display.gotoXY(0, 0);
    display.print(F("Speed: "));
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0, 1);
    display.print("Distance; ");
    display.print(totDis);
    buttonDisplay();
}

void screenBatteryHealth()
{
    display.gotoXY(0, 0);
    display.print("Battery_Level: ");
    display.println(battery_health);
    display.gotoXY(0, 1);
    display.print("Charging_Cycles: ");
    display.println(chargesCounter);
    buttonDisplay();
}

void alarm10()
{
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    buttonDisplay();
    ledYellow(1);
    buzzer.stopPlaying();
}

void alarm5()
{
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.println("Battery health");
    display.print(battery_health);
    display.gotoXY(0, 1);
    display.println("Battery low!");
    buttonDisplay();
    ledRed(1);
    motors.setSpeeds(0, 0);
    buzzer.playFrequency(440, 200, 15);
    delay(1000);
    buzzer.stopPlaying();
    aMillis = millis();
}

void emptyBattery()
{
    motors.setSpeeds(0, 0);
    display.print("Battery empty");
    display.gotoXY(0, 1);
    display.print("Please change battery");
}

void BatteryHealthCheck()
{
    int mistake = 1;
    int mistakeCheck = random(100);
    if (mistake == mistakeCheck)
        mistake = 2;
    battery_health = (battery_health - chargesCounter - fiveLevelCounter - over70Counter - averageSpeed() / 10 - maxSpeed / 10) / mistake;
    if (battery_health < 0)
        battery_health = 0;
    EEPROM.write(0, battery_health);
}

void batteryService()
{
    motors.setSpeeds(0, 0);
    battery_health += 10;
    chargesCounter++;
    bankBalance -= 10;
    display.clear();
    display.println("Battery charged");
    display.println(battery_health);
    display.print("%");
    display.gotoXY(0, 2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(2000);
    display.clear();
}

void pressA()
{
    if (buttonA.isPressed())
    {
        v = 5;
    }
}

void pressB()
{
    if (buttonB.isPressed())
    {
        v = 6;
    }
}

void batteryChange()
{
    motors.setSpeeds(0, 0);
    battery_health = 100;
    chargesCounter = 0;
    fiveLevelCounter = 0;
    over70Counter = 0;
    bankBalance -= 100;
    display.clear();
    display.print("Battery changed!");
    display.gotoXY(0, 2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(2000);
    display.clear();
}

void mainFunction()
{
    SpeedValues();
    pressA();
    pressB();
    switch (v)
    {
    case 0:
        screenSpeedometer();
        
        if (battery_health <= 0)
        {
            v = 7;
            display.clear();
            break;
        }
        if (millis() - lMillis >= 15000)
        {
            if (battery_health < 10)
            {
                v = 3;
            }
            if (battery_health < 5)
            {
                v = 4;
            }
            lMillis=millis();
            break;
        } 
        if (millis() - sMillis >= 10000)
        {
            v = 1;
            sMillis = millis();
            break;
        }
        break;

    case 1:
        screenBatteryHealth();
        if (millis() - sMillis >= 1000)
        {
            v = 0;
            display.clear();
        }
        break;

    case 3:
        alarm10();
        v = 0;
        break;

    case 4:
        alarm5();
        v = 0;
        break;

    case 5:
        batteryService();
        v = 0;
        break;

    case 6:
        batteryChange();
        v = 0;
        break;
    case 7:
        emptyBattery();
        if (battery_health > 0)
        {
            v = 0;
        }
    }
}

void setup()
{
    display.clear();
    display.setLayout21x8();
}

void loop()
{
    motors.setSpeeds(200, 200);
    mainFunction();
}
