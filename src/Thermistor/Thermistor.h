#ifndef THERMISTOR_H
#define THERMISTOR_H

#define THERMISTOR_SAMPLES_NUM 64
#define THERMISTOR_SAMPLES_DELAY_MS 1
#define THERMISTOR_CELSIUS_SHIFT 273.15

#include <Arduino.h>

// Read http://arduino-diy.com/arduino-thermistor
// for details of realization.
class Thermistor
{
    public:
        Thermistor(
            uint8_t pin, 
            float nominalResistanceKohm, 
            float nominalTemperature,
            float bCoeff, 
            float cCoeff, 
            float seriesResistanceKohm
        );
        float ReadTemperature();					  
										  
    private:
        uint8_t pin;
        float pinMaxValue;
        float nominalResistanceKohm;
        float nominalTemperature;
        float bCoeff;
        float cCoeff;
        float seriesResistanceKohm;

        float ReadResistance();
        float ConvertResistanceToTemperature(float R);
};


#endif