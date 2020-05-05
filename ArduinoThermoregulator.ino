#include <SPI.h>
#include "src/Adafruit/Adafruit_GFX.h"
#include "src/Adafruit/Adafruit_PCD8544.h"
#include "src/Keypad/Keypad.h"

const byte ROWS = 4; // 4 строки
const byte COLS = 4; // 4 столбца
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
}; 
byte rowPins[ROWS] = {18, 19, 20, 21}; 
byte colPins[COLS] = {14, 15, 16, 17}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)

// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

void printNumber(unsigned int n) {
  if (n < 10) {
    display.print(" ");
  }
  
  if (n < 100) {
    display.print(" ");
  }
  
  display.print(n);
}


void print(unsigned int s1, unsigned int s2, unsigned int rq) {
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("SENSORS:");
  
  display.setTextSize(2);
  printNumber(s1);
  display.print(" ");
  printNumber(s2);
  display.println("");

  display.setTextSize(1);
  display.println("REQUIRED:");
  
  display.setTextSize(2); 
  printNumber(rq);

  display.display();
}

void printEnterNumber(unsigned int n) {
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("SET REQUIRED:");
  
  display.setTextSize(2);

  if (n == 0) {
    display.println("");
  } else {
    display.println(n);
  }
  
  display.setCursor(0, 24);
  display.setTextSize(1);
  display.println("");
  display.println("A - SUBMIT");
  display.println("B - CANCEL");

  display.display();
}

unsigned int convertKeyToNumber(char key) {
  switch (key) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
  }

  return 10;
}



void setup() {
  display.begin();
  display.setContrast(50);
  display.setTextColor(BLACK); 
  
  randomSeed(666);
}

int i = 0;

// Mode: 
// - 0 Display sensors and required temp
// - 1 Set required temp
unsigned int mode = 0;

int rq = random(300);
int s1 = 0;
int s2 = 0;

int userRq = 0;

void loop() {
  s1 = (int) readTemperature1();
  s2 = (int) readTemperature2();
  
  switch (mode) {
    case 0:
      print(s1, s2, rq);
      break;

    case 1:
      printEnterNumber(userRq);
      break;
  }
 
  char key = keypad.getKey();
  if (key){
    handleKey(key);
  }
}

void handleKey(char key) {
  if (key == 'D') {
    mode++;

    if (mode > 1) {
      mode = 0;
    }

    return;
  }
    
  switch (mode) {
    case 0:
      mode0key(key);
      break;

    case 1:
      mode1key(key);
      break;
  } 
}

void mode0key(char key) {
}

void mode1key(char key) {
  unsigned int number = convertKeyToNumber(key);

  if (number != 10) {
    if (userRq < 100) {
      userRq = userRq * 10 + number;  
    }
    return;
  } 
  
  switch (key) {
      case 'A':
        rq = userRq;
                
      case 'B':
        userRq = 0;
        mode = 0;
        return;
        
      case 'C':
        userRq = userRq / 10;
        return;
        
      case '*':
      case '#': 
        break;
   }
}

/**
 * ==================================================================
 * THERMISTORS FUNCTIONS
 * ==================================================================
 */
 
// http://arduino-diy.com/arduino-thermistor
#define THERMISTOR_SAMPLES_NUM 5
#define THERMISTOR_SAMPLES_DELAY_MS 20

#define THERMISTOR1_PIN A0 
#define THERMISTOR1_PIN_MAX_VALUE 1023
#define THERMISTOR1_SERIES_RESISTANCE_OHM 100
 
#define THERMISTOR1_NOMINAL 100000
#define THERMISTOR1_TEMPERATURE_NOMINAL 25
#define THERMISTOR1_BCOEFFICIENT 4036

#define THERMISTOR2_PIN A1 
#define THERMISTOR2_PIN_MAX_VALUE 1023
#define THERMISTOR2_SERIES_RESISTANCE_OHM 100
 
#define THERMISTOR2_NOMINAL 100000
#define THERMISTOR2_TEMPERATURE_NOMINAL 25
#define THERMISTOR2_BCOEFFICIENT 4036


float readThermistorResistance(uint8_t pin, uint8_t pinMaxValue, uint8_t seriesResistance) {
  float sum = 0.0d;
  
  for (int i = 0; i < THERMISTOR_SAMPLES_NUM; i++) {
    sum += analogRead(pin);
    delay(THERMISTOR_SAMPLES_DELAY_MS);
  }

  float average = sum / THERMISTOR_SAMPLES_NUM;  
  float R = seriesResistance / ((pinMaxValue / average) - 1);

  return R;
}

float convertResistanceToTemperature(float R, float R0, float T0, float B) {
  float steinhart = log(R / R0); // ln(R/Ro)
  
  steinhart /= B; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (T0 + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // инвертируем
  steinhart -= 273.15; // конвертируем в градусы по Цельсию

  return steinhart;
}

float readTemperature1() {
  float resistance = readThermistorResistance(THERMISTOR1_PIN, THERMISTOR1_PIN_MAX_VALUE, THERMISTOR1_SERIES_RESISTANCE_OHM);
  float temperature = convertResistanceToTemperature(resistance, THERMISTOR1_NOMINAL, THERMISTOR1_TEMPERATURE_NOMINAL, THERMISTOR1_BCOEFFICIENT);

  return temperature;
}

float readTemperature2() {
  float resistance = readThermistorResistance(THERMISTOR2_PIN, THERMISTOR2_PIN_MAX_VALUE, THERMISTOR2_SERIES_RESISTANCE_OHM);
  float temperature = convertResistanceToTemperature(resistance, THERMISTOR2_NOMINAL, THERMISTOR2_TEMPERATURE_NOMINAL, THERMISTOR2_BCOEFFICIENT);

  return temperature;
}
