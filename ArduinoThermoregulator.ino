#include "src/Screen/Screen.h"
#include "src/Keypad/Keypad.h"
#include "src/PIDRelay/PIDRelay.h"
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

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
}; 
byte rowPins[ROWS] = {18, 19, 20, 21}; 
byte colPins[COLS] = {14, 15, 16, 17}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Screen screen(8, 9, 10, 11, 12);

PIDRelay pidRelay(RELAY_PIN, RELAY_KP, RELAY_KI, RELAY_KD);

void setup() {
  pidRelay.Init();
  screen.Init();
  
  pinMode(FUN_PIN, OUTPUT);
}

// Mode: 
// - 0 Display sensors and required temp
// - 1 Set required temp
unsigned int mode = 0;

int requiredTemp = 20;
float sensor1Temp = 0;
float sensor2Temp = 0; 

int newRequired = 0;

void loop() {
  sensor1Temp = thermistor1.ReadTemperature();
  sensor2Temp = thermistor2.ReadTemperature();

  if (sensor1Temp > 0) {
    pidRelay.SetCurrentTemperature(sensor1Temp);
    pidRelay.ComputeAndSet();
  } else {
    pidRelay.SetRelayValue(0);
  }

  if (sensor1Temp >= FUN_MIN_TEMPERATURE || requiredTemp >= FUN_MIN_TEMPERATURE) {
    analogWrite(FUN_PIN, 255);
  } else {
    analogWrite(FUN_PIN, 0);
  }

  switch (mode) {
    case 0:
      screen.ShowCurrentState(sensor1Temp, sensor2Temp, requiredTemp);
      break;

    case 1:
      screen.ShowRequiredSetting(newRequired);
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
    if (newRequired < 100) {
      newRequired = newRequired * 10 + number;  
    }
    return;
  } 
  
  switch (key) {
      case 'A':
        requiredTemp = newRequired;
        pidRelay.SetRequireTemperature(newRequired);
                
      case 'B':
        newRequired = 0;
        mode = 0;
        return;
        
      case 'C':
        newRequired = newRequired / 10;
        return;
        
      case '*':
      case '#': 
        break;
   }
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
