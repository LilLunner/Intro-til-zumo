int ledLight[4] = {2, 3, 4, 5};
int lightSensor = A5;
int presureSensor = A3;
int trig = 6;
int echo = 7;
int pushButton = 8;

void setup()
{
    for (int i = 0; i <= 4; i++)
    {
        pinMode(ledLight[i], OUTPUT);
    }
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

void ledOn(int var)
{
    for (int i = 0; i <= 3; i++)
    {
        if (i < var)
            digitalWrite(ledLight[i], HIGH);
        else
            digitalWrite(ledLight[i], LOW);
    }
}
void allOff()
{
    for (int i = 0; i <= 3; i++)
    {
        digitalWrite(ledLight[i], LOW);
    }
}

// hentet fra program jeg har  agd tidligere i semsteret
bool button() // må initaliserer "valg" og "buttonVar" i oppstart
{
    static bool buttonVar, valg = false;
    if (digitalRead(pushButton) == HIGH)
    {
        buttonVar = true; // setter buttonVar til true mens knappen er klikket ned
    }
    if ((digitalRead(pushButton) == LOW) && (buttonVar == true)) // vil kjøre når knappen slippes og endrer retur variablen
    {
        valg = !valg;
        buttonVar = false;
    }
    return valg;
}

int pressCount()
{
    static bool lastButtonState = false;
    static int counter;
    if (lastButtonState != button())
    {
        lastButtonState = button();
        counter++;
    }
    if (counter > 2)
        counter = 0;
    return counter;
}

void LightSensorShow()
{
    int lightSensorVal = map(analogRead(lightSensor), 685, 997, 4, 0);
    ledOn(lightSensorVal);
}

void ryggeSensor()
{
    static float dis;
    digitalWrite(trig, LOW);
    delayMicroseconds(2); // kilde https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1
    digitalWrite(trig, HIGH);
    dis = (pulseIn(echo, HIGH) * 0.0343) / 2;

    if (dis <= 20)
    {
        int disShow = map(dis, 0, 20, 4, 0);
        ledOn(disShow);
    }
    else
    {
        allOff();
    }
}

void crashDetect()
{
    int pressure = map(analogRead(presureSensor), 0, 1023, 0, 4);
    switch (pressure) // måten vi har koblet opp, er LED på pin 3 og 4 grønne, 5 gul og 2 rød, så basert på trykket, vil flere LED lyse i økende fare lys,
    {                 // for  å representere skaden
    case 0:
        allOff();
        break;
    case 1:
        digitalWrite(ledLight[1], HIGH);
        digitalWrite(ledLight[2], HIGH);
        break;
    case 2:
        digitalWrite(ledLight[3], HIGH);
        break;
    case 3:
        digitalWrite(ledLight[0], HIGH);
    }
    Serial.println(pressure);
}

void mode(){
    switch (pressCount())
    {
    case 0:
        LightSensorShow();
        break;
    case 1:
        ryggeSensor();
        break;
    case 2:
        crashDetect();
    }
}

void loop()
{
    mode();
}