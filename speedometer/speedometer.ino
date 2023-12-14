#include <Zumo32U4.h>
#include <EEPROM.h>
#include <Wire.h>
Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;
#include "TurnSensor.h"

int topSpeed = 200;

int SpeedArray[60], totDis, maxSpeed;
int arrayIndex = -1; //Starter som -1, men blir med en gang gjort om til 0 i DistancePerSecond funksjonen.
int over70Counter, chargesCounter, fiveLevelCounter, v, arraySum;
int battery_health = EEPROM.read(0);
int bankBalance = EEPROM.read(1);

int distance() // Teller antall motorrotasjoner og omformer det til cm.
{
    long int L = encoder.getCountsAndResetLeft();
    long int R = encoder.getCountsAndResetRight();

    long int dis = (L + R) * 6 / (910); // cm
    return dis;
}

void DistancePerSecond() // Oppdaterer en indeks i et 60 tall langt array som måler distanse per sekund, og sjekker om farten er over 70% av makshastighet.
{
    arrayIndex++;
    if (arrayIndex == 60)
        arrayIndex = 0;
    arraySum = arraySum - SpeedArray[arrayIndex]; // Trekker fra den gamle arrayverdien fra totalsummen.
    if (SpeedArray[arrayIndex] >= 49)
        over70Counter--;
    SpeedArray[arrayIndex] = distance(); // Her byttes den gamle arrayverdien med den nye.
    if (SpeedArray[arrayIndex] >= 49)
        over70Counter++;
    arraySum = arraySum + SpeedArray[arrayIndex];
}

void totalDistance() // Regner ut totaldistanse.
{
    totDis = totDis + SpeedArray[arrayIndex];
}

int averageSpeed() // regner ut gjennomsnittshastighet.
{
    int arrayValuesCounter;
    for (int i = 0; i < 60; i++)
    {
        if (SpeedArray[i] != 0)
            arrayValuesCounter++;
    }
    int average = arraySum / arrayValuesCounter;
    return average;
}

void highestSpeed() // Sjekker makshastigheten.
{
    maxSpeed = 0;
    for (int i; i <= 59; i++)
    {
        if (SpeedArray[i] > maxSpeed)
            maxSpeed = SpeedArray[i];
    }
}

void buttonDisplay() // Displayet som viser hva knappetrykk gjør.
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

void SpeedValues() // Oppdaterer alle hastighetsrelaterte verdiene hvert sekund og hvert minutt.
{
    static unsigned long cMillis, mMillis = millis();
    if (millis() - cMillis >= 1000) // Kjører kun hvert sekund slik at en arrayverdi blir oppdatert per sekund.
    {

        static bool x = 1;
        BatteryHealthCheck();
        DistancePerSecond();
        totalDistance();
        cMillis = millis();
        if (x == 1 && SpeedArray[arrayIndex] != 0)
        {
            maxSpeed = SpeedArray[arrayIndex];
            x = 0;
        }
        if (millis() - mMillis >= 60000)
        {
            highestSpeed();
            mMillis = millis();
        }
    }
}

void screenSpeedometer() // Hva som vises på skjermen av speedometeret
{
    display.gotoXY(0, 0);
    display.print(F("Speed: "));
    display.println(SpeedArray[arrayIndex]);
    display.gotoXY(0, 1);
    display.print("Distance; ");
    display.print(totDis);
    buttonDisplay();
}

void screenBatteryHealth() // Hva som vises på skjermen av batterihelsen.
{
    display.gotoXY(0, 0);
    display.print("Battery_Level: ");
    display.println(battery_health);
    display.gotoXY(0, 1);
    display.print("Charging_Cycles: ");
    display.println(chargesCounter);
    buttonDisplay();
}

void alarm10() // Alarmen som slår ut når batteriet er under 10%.
{
    buzzer.playFrequency(440, 200, 15);
    display.clear();
    display.print("Battery low!");
    buttonDisplay();
    ledYellow(1);
    buzzer.stopPlaying();
}

void alarm5() // Alarmen som slår ut når batteriet er under 5%.
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
    delay(300); // Delay brukes bevisst for at bilen ikke kjører og at tonen skal høres.
    buzzer.stopPlaying();
    delay(300);
    buzzer.playFrequency(440, 200, 15);
}

void emptyBattery() // Sørger for at bilen stopper ved tomt batteri.
{
    motors.setSpeeds(0, 0);
    display.print("Battery empty");
    display.gotoXY(0, 1);
    display.print("Please change battery");
}

void BatteryHealthCheck() // Regner ut batterihelsen og lagrer den i EEPROM.
{
    int mistake = 1;
    int mistakeCheck = random(100);
    if (mistake == mistakeCheck)
        mistake = 2;
    battery_health = (battery_health - chargesCounter - fiveLevelCounter - over70Counter - averageSpeed() / 10 - maxSpeed / 10) / mistake; //Om gjennomsnittshastigheten og topphastigheten er negativ, vil batteriet lades.
    if (battery_health < 0)
        battery_health = 0;
    EEPROM.write(0, battery_health);
}

void batteryCharging() // Lader batteriet.
{
    motors.setSpeeds(0, 0);
    battery_health += 30;
    if (battery_health > 100)
        battery_health = 100;
    chargesCounter++;
    bankBalance -= 10;
    EEPROM.write(0, battery_health); // Oppdaterer bankbalansen og batterhelsen i EEPROM.
    EEPROM.write(1, bankBalance);
    display.clear();
    display.println("Battery charged");
    display.println(battery_health);
    display.print("%");
    display.gotoXY(0, 2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(1000);
    display.clear();
}

void pressA() // Hva som skjer om man trykker på A.
{
    if (buttonA.isPressed())
    {
        v = 5;
    }
}

void pressB() // Hva som skjer om man trykker på B.
{
    if (buttonB.isPressed())
    {
        v = 6;
    }
}

void batteryChange() // Bytter batteriet.
{
    motors.setSpeeds(0, 0);
    battery_health = 100;
    chargesCounter = 0;
    fiveLevelCounter = 0;
    over70Counter = 0;
    bankBalance -= 100;
    EEPROM.write(1, bankBalance);
    display.clear();
    display.print("Battery changed!");
    display.gotoXY(0, 2);
    display.println("Bank balance;");
    display.print(bankBalance);
    delay(1000);
    display.clear();
}

void mainFunction() // Tar inn all funksjonene og organiserer dem i en stor switch-case.
{
    static unsigned long lMillis, sMillis = millis();
    SpeedValues(); // Disse kjøres utenfor switch-casen fordi man alltid vil kunne lade batteriet og oppdatere hastighetensverdiene.
    pressA();
    pressB();
    switch (v)
    {
    case 0:
        screenSpeedometer();
        // Kjører speedometeret på skjermen og sjekker om batteriet er tomt eller om det er på tide å vise batterihelsen eller alarm for lavt batteri.
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
                display.clear();
                v = 3;
            }
            if (battery_health < 5)
            {
                display.clear();
                v = 4;
            }
            lMillis = millis();
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
        screenBatteryHealth(); // Viser batterihelsen i et sekund og går deretter tilbake til speedometert.
        if (millis() - sMillis >= 1000)
        {
            v = 0;
            display.clear();
            sMillis = millis();
        }
        break;

    case 3:
        alarm10(); // Gir 10% alarm og går deretter tilbake til speedometeret.
        if (millis() - lMillis >= 1000)
        {
            v = 0;
            buzzer.stopPlaying();
            lMillis = millis();
        }
        break;

    case 4:
        alarm5(); // Gir 5% alarm og går deretter tilbake til speedometeret.
        if (millis() - lMillis >= 1000)
        {
            v = 0;
            buzzer.stopPlaying();
            lMillis = millis();
        }
        break;

    case 5:
        batteryCharging(); // Lader batteriet og går deretter tilbake til speedometeret.
        v = 0;
        break;

    case 6:
        batteryChange(); // Bytter batteriet og går deretter tilbake til speedometeret.
        v = 0;
        break;
    case 7:
        emptyBattery(); // Stopper bilen og står der til batteriet blir ladet eller byttet.
        if (battery_health > 0)
        {
            v = 0;
        }
    }
}

void turnDeg(int x, int y) // x er antal rotasjoner, y er vinkel
{
    int i = 1;
    while (i <= x) // teller antall rotasjoner
    {
        motors.setSpeeds(-100, 100);
        while ((int32_t)turnAngle < (turnAngle1 * (y))) // stopper her helt til den har truffet 90*, det virker som hva som hva som er 90 endres fra dag til dag
        {
            turnSensorUpdate();
            lineSensors.calibrate();
        }
        turnSensorReset(); // reseter gyroskop dataen
        motors.setSpeeds(0, 0);
        i++;
    }
}

int lineSensorRead()
{
    static unsigned int lineSensorVal[5]; // lager en variable med like mange indekser som det er sensorer
    int error = map(lineSensors.readLine(lineSensorVal), 0, 4000, -2000, 2000);
    return error;
}

void lineFollowPID(int pos)
{ // tar inn posisjonen
    static int prevPos;
    int correction = pos / 4 + 6 * (pos - prevPos); // kilde eksempelkode
    prevPos = pos;
    int lSpeed = topSpeed + correction;      // farten på venstre side lik topSpeed + correction
    int rSpeed = topSpeed - correction;      // farten på høgre side lik topspeed - correction
    lSpeed = constrain(lSpeed, 0, topSpeed); // setter slik at verdien vil alltids være mellom 200 og 0, vil forhindre for høye hastigheter, men viktigs
    rSpeed = constrain(rSpeed, 0, topSpeed); // hindrer at det vil fort gå fra positiv hastighet til negativ hastighet som kan skade motorene.
    motors.setSpeeds(lSpeed, rSpeed);
}

void setup()
{
    display.clear();
    display.setLayout21x8();
    lineSensors.initFiveSensors();
    turnSensorSetup();
    turnDeg(4, 90);
}

void loop()
{
    lineFollowPID(lineSensorRead());
    mainFunction();
}
