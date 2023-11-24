int totdiss=10;

int battery(totdiss) {
    int power = 100-totdiss*4;
    return power;
}

int charge() {
    int power = battery(totdiss) + 1;
    return power;
}

void setup() {

}

void loop() {

}