#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include "../Adafruit/Adafruit_GFX.h"
#include "../Adafruit/Adafruit_PCD8544.h"

class Screen
{
    public:
        Screen(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);

        void Init();

        void ShowCurrentState(float sensor1, float sensor2, int required);
        void ShowRequiredSetting(int newRequired);	  
										
    private:
        Adafruit_PCD8544 display;
};


#endif