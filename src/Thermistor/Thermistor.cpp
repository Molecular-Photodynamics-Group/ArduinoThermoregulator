#include "Thermistor.h"

Thermistor::Thermistor(
    uint8_t pin, 
    float nominalResistanceKohm, 
    float nominalTemperature,
    float bCoeff, 
    float seriesResistanceKohm
) {
    this->pin = pin;
    this->nominalResistanceKohm = nominalResistanceKohm;
    this->nominalTemperature = nominalTemperature;
    this->bCoeff = bCoeff;
    this->seriesResistanceKohm = seriesResistanceKohm;

    this->pinMaxValue = 1023.0f;

    pinMode(pin, INPUT);
}

float Thermistor::ReadResistance() {
  uint16_t sum = 0;
  
  for (int i = 0; i < THERMISTOR_SAMPLES_NUM; i++) {
    sum += analogRead(pin);
    delay(THERMISTOR_SAMPLES_DELAY_MS);
  }

  float average = ((float) sum) / THERMISTOR_SAMPLES_NUM;  
  float R = seriesResistanceKohm / ((pinMaxValue / average) - 1.0);

  return R;
}

float Thermistor::ConvertResistanceToTemperature(float R) {
    float steinhart = logf(R / nominalResistanceKohm); // ln(R/Ro)

    steinhart /= bCoeff; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (nominalTemperature + THERMISTOR_CELSIUS_SHIFT); // + (1/To)
    steinhart = 1.0 / steinhart; // инвертируем
    steinhart -= THERMISTOR_CELSIUS_SHIFT; // конвертируем в градусы по Цельсию

    return steinhart;
}

float Thermistor::ReadTemperature() {
    float resistance = ReadResistance();
    float temperature = ConvertResistanceToTemperature(resistance);

    return temperature;
}