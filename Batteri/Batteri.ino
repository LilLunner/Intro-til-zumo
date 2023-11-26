int totdiss=10;
int charging_cycles;

int battery(totdiss) {
    int power = 100-totdiss*4;
    return power;
}

int charge() {
    int power = battery(totdiss) + 1;
    charging_cycles++;
    return power;
}

int health(power, charging_cycles) {
    
}

void setup() {

}

void loop() {

}