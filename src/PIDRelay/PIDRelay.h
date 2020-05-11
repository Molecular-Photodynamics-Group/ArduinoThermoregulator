
#ifndef PID_RELAY_H
#define PID_RELAY_H

#include <Arduino.h>
#include "../PID/PID.h"

class PIDRelay
{
    public:
        PIDRelay(uint8_t relayPin, double Kp, double Ki, double Kd);

        void Init();

        void SetRequireTemperature(double temperature);
        void SetCurrentTemperature(double temperature);
        
        void ComputeAndSet();
    private:
        PID pid;
        uint8_t relayPin;
        double relayOutput;
        double requiredTemperature;
        double currentTemperature;
};


#endif