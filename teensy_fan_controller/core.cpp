//
// Created by jd on 11/26/2019.
//

#include <EEPROM.h>
#include "core.h"


#ifndef DISABLE_EEPROM
static_assert(E2END + 1 >= CONFIG_BYTES, "Device EEPROM size must be at least 127 bytes");
#endif


FanData::FanData(const RuntimeConfig::FanConfig &fan, const String &lbl) : cfg(fan), lbl(lbl)
{
}

void FanData::setupPin(void (*isr)())
{
  pulse_counter = 0;
  if (cfg.pinPWM) {
    pinMode(cfg.pinPWM, OUTPUT);
  }
  if (isrPin) {
    detachInterrupt(digitalPinToInterrupt(isrPin));
    isrPin = 0;
  }
  if (cfg.pinRPM) {
    pinMode(cfg.pinRPM, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(cfg.pinRPM), isr, FALLING);
    isrPin = cfg.pinRPM;
  }
}

void FanData::doRPM()
{
  if (cfg.pinRPM) {
    // flush pulse_counter to RPM (2 pulse per revolution, over 0.5s...)
    rpmAvg(pulse_counter * 60);
    rpm = rpmAvg;
    pulse_counter = 0;
  }
}

void FanData::writePWM(const uint8_t pout, const CONTROL_MODE mode) const
{
  if (cfg.mode == mode && cfg.pinPWM) {
    analogWrite(cfg.pinPWM, (int)(pout * cfg.ratio));
  }
}


SensorData::SensorData(const RuntimeConfig::SensorConfig &sensor, const String &lbl) : cfg(sensor), lbl(lbl)
{
}

void SensorData::doSample()
{
  if (cfg.pin) {
    val = convert_reading(getAverage(), cfg.nominalR, TEMPERATURENOMINAL, cfg.beta, cfg.seriesR);
  }
  else {
    val = 0;
  }
}

/**
   Average the samples to a float value.

   Source: https://learn.adafruit.com/thermistor/using-a-thermistor
*/
float SensorData::getAverage() const
{
  // average all the samples out
  float average = 0;
  size_t i;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;
  return average;
}

/**
   Converts an ADC reading to degrees C.

   Source: https://learn.adafruit.com/thermistor/using-a-thermistor

   @param nominalR Resistance at nominal temperature (25 degrees C)
   @param nominalTemp Temperature for nominal resistance (almost always 25 C)
   @param beta Beta coefficient of the thermistor (usually 3000-4000)
   @param seriesR Value of the 'other' resistor
*/
/*static */float SensorData::convert_reading(float reading, uint16_t nominalR, uint8_t nominalTemp, uint16_t beta, uint16_t seriesR)
{
  // convert value1 to resistance
  reading = (ADC_RANGE / reading) - 1;
  reading = seriesR / reading;

  // convert resistance to degC
  reading = reading / nominalR;             // (R/Ro)
  reading = log(reading);                   // ln(R/Ro)
  reading /= beta;                          // 1/B * ln(R/Ro)
  reading += 1.0 / (nominalTemp + 273.15);  // + (1/To)
  reading = 1.0 / reading;                  // Invert
  reading -= 273.15;
  return reading;
}


int read_config(byte bytes[], size_t len)
{
  memset(bytes, '\0', len);
  uint16_t i;
  for (i = 0; i < len; i++) {
    *(bytes + i) = EEPROM[i];  //EEPROM.read(i)
  }
  return 0;
}

int write_config(const byte bytes[], size_t len)
{
  uint16_t i;
  for (i = 0; i < len; i++) {
    EEPROM[i] = bytes[i];  //EEPROM.write(i, val)
  }
  return 0;
}
