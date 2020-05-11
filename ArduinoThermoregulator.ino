#include <SPI.h>
#include "src/Adafruit/Adafruit_GFX.h"
#include "src/Adafruit/Adafruit_PCD8544.h"
#include "src/Keypad/Keypad.h"
#include "src/PID/PID.h"
#include "src/Thermistor/Thermistor.h"
#include "Constants.h"

Thermistor thermistor1 = Thermistor(
  THERMISTOR1_PIN, 
  THERMISTOR1_NOMINAL_RESISTANCE_KOHM, 
  THERMISTOR1_NOMINAL_TEMPERATURE,
  THERMISTOR2_BCOEFFICIENT,
  THERMISTOR1_SERIES_RESISTANCE_KOHM
);

Thermistor thermistor2 = Thermistor(
  THERMISTOR2_PIN, 
  THERMISTOR2_NOMINAL_RESISTANCE_KOHM, 
  THERMISTOR2_NOMINAL_TEMPERATURE,
  THERMISTOR2_BCOEFFICIENT,
  THERMISTOR2_SERIES_RESISTANCE_KOHM
);

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


#define RELAY_PIN 5

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 15;
double Ki = 0.5;
double Kd = 0;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


void printNumber(double n) {
  if (n < 10) {
    display.print(" ");
  }
  
  if (n < 100) {
    display.print(" ");
  }
  
  display.print(n);
}


void print(double s1, double s2, double rq) {
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("SENSORS:");
  
  display.setTextSize(1);
  printNumber(s1);
  display.print(" ");
  printNumber(s2);
  display.println("");
  display.println("");

  display.setTextSize(1);
  display.println("REQUIRED:");
  
  display.setTextSize(2); 
  printNumber(rq);

  display.display();
}

void printEnterNumber(int n) {
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
  Serial.begin(9600);
  
  TCCR3B = TCCR3B & B11111000 | B00000101;    // set timer 3 divisor to  1024 for PWM frequency of    30.64 Hz

  pinMode(RELAY_PIN, OUTPUT);
  
  analogWrite(RELAY_PIN, 0);

  myPID.SetOutputLimits(0, 255);
  myPID.SetMode(AUTOMATIC);

  display.begin();
  display.setContrast(50);
  display.setTextColor(BLACK); 
}

int i = 0;

// Mode: 
// - 0 Display sensors and required temp
// - 1 Set required temp
unsigned int mode = 0;

double rq = 20;
double s1 = 0;
double s2 = 0; 

int userRq = 0;

void loop() {
  s1 = thermistor1.ReadTemperature();
  s2 = thermistor2.ReadTemperature();
  
  Setpoint = rq;
  Input = s1;
  myPID.Compute();
  Serial.println(Output);
  analogWrite(RELAY_PIN, Output);

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
