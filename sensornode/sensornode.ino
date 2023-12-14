int ledLight[4] = {2, 3, 4, 5}; //setter opp et array for alle LED lysene
int lightSensor = A5;
int presureSensor = A3;
int trig = 6;
int echo = 7; //trig og echo pinen til den ultrasoniske sensoren
int pushButton = 8; 

void setup()
{
    for (int i = 0; i <= 4; i++) //iterer igjennom alle indeksene i LED arayet og gjør dem til utgangsporter
    {
        pinMode(ledLight[i], OUTPUT); 
    }
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

void ledOn(int var) //skrur på LEDene basert på intatt sensor data med verdi fra 0-3
{
    for (int i = 0; i <= 3; i++) //tar inputett sensor data og skrur på alle LEDene som er lik eller mindre høyeste målte sensorverdi,
    {
        if (i < var)
            digitalWrite(ledLight[i], HIGH);
        else
            digitalWrite(ledLight[i], LOW);
    }
}

void allOff() //skrur av alle LED lysene
{
    for (int i = 0; i <= 3; i++)
    {
        digitalWrite(ledLight[i], LOW);
    }
}

// hentet fra program jeg har  agd tidligere i semsteret
bool button() //registerer knappeklikk, når knappen slippes
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

int pressCount() //teller hvor mange ganger knappen har blitt klikket
{
    static bool lastButtonState = false;
    static int counter;
    if (lastButtonState != button()) //om det har blitt registrert ett klikk vil tellevariabelen øke med 1
    {
        lastButtonState = button();
        counter++;
    }
    if (counter > 2) //om tellevariabelen blir høyere enn 2 endres den tilbake til 0
        counter = 0;
    return counter;
}

void LightSensorShow()
{
    int lightSensorVal = map(analogRead(lightSensor), 685, 997, 4, 0); //tar inn data fra photoresistor og endrer de til å være fra 4-0, topp og bunnverdi har blitt testett frem til
    ledOn(lightSensorVal);
}

void ryggeSensor()
{
    static float dis;
    digitalWrite(trig, LOW); //sender ut signal hvert 2 microsekund som blir 
    delayMicroseconds(2); // kilde https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1
    digitalWrite(trig, HIGH);
    dis = (pulseIn(echo, HIGH) * 0.0343) / 2; //plukker opp echoet fra signalet til trig, ganges med en konstant for å oversette til cm

    if (dis <= 20) //om distansen er 20cm eller midre skal LED lysene lyse, ett flere lys lyses opp hver 5cm objektet næremer seg sensoren
    {
        int disShow = map(dis, 0, 20, 4, 0); //mapper ut distancen fra 20 cm, til 4 verdier slik at det blir en høgre hver 5 cm
        ledOn(disShow);
    }
    else 
    {
        allOff();
    }
}

void crashDetect() //detekterer kræsj og hvor hardt det kræsjet var
{
    int pressure = map(analogRead(presureSensor), 0, 1023, 0, 4);
    switch (pressure) // måten vi har koblet opp, er LED på pin 3 og 4 grønne, 5 gul og 2 rød, så basert på trykket, vil flere LED lyse i økende fare lys,
    {                 // for  å representere skaden
    case 0: //om ingen kræsj, ingen lys
        allOff();
        break;
    case 1: //svakt kræsj, grønne lys
        digitalWrite(ledLight[1], HIGH);
        digitalWrite(ledLight[2], HIGH);
        break;
    case 2: //medium kræsj, grønne og gule lys
        digitalWrite(ledLight[3], HIGH);
        break;
    case 3: //hardt kræsj, grønne, gule og røde lys
        digitalWrite(ledLight[0], HIGH);
    }
}

void mode(){ //velger hvilken sensor som skal vises basert på antall knappeklikk
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