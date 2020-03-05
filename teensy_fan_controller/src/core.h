//
// Created by jd on 11/26/2019.
//

#ifndef TFC_CORE_H
#define TFC_CORE_H

#include <array>
#include <Arduino.h>
#include <EEPROM.h>
#include <PID_v1.h>
#include "moving_average.h"
#include "runtime_config.h"


//#define DISABLE_EEPROM  // disable EEPROM
#define USB_RAWHID_EN  // enable RawHID

// Controller constants:
#define FAN_CNT 6
#define PERIOD_UPDATE 100  // how often to read ADC / update PID
#define PERIOD_RPM 500

// ADC coefficients:
#define ADC_RESOLUTION 12
#define ADC_RANGE 4096
#define NUMSAMPLES 10  // num of samples to averages
#define READ_DELAY 1  // delay between ADC reads


// disable EEPROM if device EEPROM is too small:
#ifndef DISABLE_EEPROM
#if E2END + 1 < CONFIG_BYTES
#define EEPROM_TOO_SMALL
#define DISABLE_EEPROM
#endif  //E2END + 1 < CONFIG_BYTES
#endif  //DISABLE_EEPROM


/**
   Fan setup, RPM calculation and storage, for a single fan.
*/
struct FanData {
  const RuntimeConfig::FanConfig &cfg;

  uint16_t rpm = 0;
  Moving_Average<uint16_t, uint16_t, 5> rpmAvg;
  uint16_t pulse_counter = 0;

  uint8_t isrPin = 0;
  uint8_t pwmPin = 0;

  const String lbl;

  FanData(const RuntimeConfig::FanConfig &fan, const String &lbl);

  void setupPin(void (*isr)());
  void doRPM();
  void writePWM(const uint8_t pout, const bool useRatio) const;
};


/**
   Sensor setup, and calculation for actual reading, for a single temperature sensor.
*/
struct SensorData {
  const RuntimeConfig::SensorConfig &cfg;

  uint16_t samples[NUMSAMPLES];
  float val = 0;

  uint8_t adcPin = 0;

  const String lbl;

  SensorData(const RuntimeConfig::SensorConfig &sensor, const String &lbl);

  void setupPin();
  void doSample();
  float getAverage() const;

  static float convert_reading(float reading, const uint16_t nominalR, const uint8_t nominalTemp, const uint16_t beta, const uint16_t seriesR);
};


/**
   Read EEPROM into bytes[].
*/
int read_config(byte bytes[], const uint16_t len);

/**
   Write bytes[] into EEPROM.
*/
int write_config(const byte bytes[], const uint16_t len);


#endif //TFC_CORE_H
