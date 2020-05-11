
#include "PIDRelay.h"

PIDRelay::PIDRelay(uint8_t relayPin, double Kp, double Ki, double Kd)
    : pid(&currentTemperature, &relayOutput, &requiredTemperature, Kp, Ki, Kd, DIRECT) {
    this->relayPin = relayPin;
}

void PIDRelay::Init() {
    // set PWM frequency to 30.64 Hz
    if (relayPin == 4 || relayPin == 13) {
        TCCR0B = TCCR0B & B11111000 | B00000101;
    }
    if (relayPin == 11 || relayPin == 12) {
        TCCR1B = TCCR1B & B11111000 | B00000101;
    }
    if (relayPin == 9 || relayPin == 10) {
        TCCR2B = TCCR2B & B11111000 | B00000111;
    }
    if (relayPin == 2 || relayPin == 3 || relayPin == 5) {
        TCCR3B = TCCR3B & B11111000 | B00000101;
    }
    if (relayPin == 6 || relayPin == 7 || relayPin == 8) {
        TCCR3B = TCCR3B & B11111000 | B00000101;
    }

    pinMode(relayPin, OUTPUT);    
    analogWrite(relayPin, 0);

    pid.SetOutputLimits(0, 255);
    pid.SetMode(AUTOMATIC);
}

void PIDRelay::SetRequireTemperature(double temperature) {
    requiredTemperature = temperature;
}

void PIDRelay::SetCurrentTemperature(double temperature) {
    currentTemperature = temperature;
}

void PIDRelay::ComputeAndSet() {
    pid.Compute();

    analogWrite(relayPin, relayOutput);
}