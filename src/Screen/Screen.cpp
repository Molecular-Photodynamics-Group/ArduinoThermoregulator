#include "Screen.h"

Screen::Screen(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST) : display(SCLK, DIN, DC, CS, RST) {
}

void Screen::Init() {
    display.begin();
    display.setContrast(100);
    display.setTextColor(BLACK); 
}

void Screen::ShowCurrentState(float sensor1, float sensor2, int required) {
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("SENSORS:");
  
  display.setTextSize(2);
  display.print(round(sensor1 * 10) / 10.0, 1);

  display.setCursor(66, 15);
  display.setTextSize(1);
  display.print(sensor2, 0);

  display.setCursor(0, 25);
  display.setTextSize(1);
  display.println("REQUIRED:");
  
  display.setTextSize(2); 
  display.println(required);

  display.display();
}

void Screen::ShowRequiredSetting(int newRequired) {
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("SET REQUIRED:");
  
  display.setTextSize(2);

  if (newRequired == 0) {
    display.println("");
  } else {
    display.println(newRequired);
  }
  
  display.setCursor(0, 24);
  display.setTextSize(1);
  display.println("");
  display.println("A - SUBMIT");
  display.println("B - CANCEL");

  display.display();
}