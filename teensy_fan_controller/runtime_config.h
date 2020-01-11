//
// Created by jd on 11/26/2019.
//

#ifndef TFC_RUNTIMECONFIG_H
#define TFC_RUNTIMECONFIG_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <cstddef>
typedef unsigned char byte;
#define A4 0
#define A6 0
#define A7 0
#endif


// thermistor coefficients/defaults
#define TEMPERATURENOMINAL 25  // temp. for thermistor nominal resistance
#define DEFAULT_BCOEFFICIENT 3950  // Default beta coefficient of the thermistor (usually 3000-4000)
#define DEFAULT_SERIESRESISTOR 9377  // Default value of the 'other' resistor
#define DEFAULT_THERMISTORNOMINAL 10000  // Default thermistor nominal resistence (at nominal temp)

#define CONTROLLER_VERSION 1  // use to flag the version of configuration saved
#define CONFIG_KEY1 0x1B  // use to flag a valid config
#define CONFIG_KEY2 0x30  // use to flag a valid config

#define CONFIG_POS_VERSION              0
#define CONFIG_POS_KEY1                 1
#define CONFIG_POS_KEY2                 2
#define CONFIG_POS_CONFIG               4

#define CONFIG_BYTES 448
#define CHUNK_SIZE 56
#define CHUNK_CNT 8


enum class CONTROL_MODE : uint8_t {
  MODE_TBL,
  MODE_PID,
  MODE_FIXED,
  MODE_OFF
};

enum class CONTROL_SOURCE : uint8_t {
  SENSOR_WATER_SUPPLY_TEMP,
  SENSOR_WATER_RETURN_TEMP,
  SENSOR_CASE_TEMP,
  SENSOR_AUX1_TEMP,
  SENSOR_AUX2_TEMP,
  VIRTUAL_DELTA_TEMP,
};


/**
   Fan controller's runtime configuration struct.
*/
struct RuntimeConfig {
  struct TableConfig {
    float temp_pct_table[10][2];
  };
  struct FanConfig {
    uint8_t pinPWM;
    uint8_t pinRPM;
    CONTROL_MODE mode;
    CONTROL_SOURCE source;
    float ratio;
    TableConfig tbl;
  };
  struct PIDConfig {
    struct PIDStep {
      uint8_t pct;
      uint16_t delay;  // # of seconds fan % must be below step_down_pct to step setpoint down 0.5C
      float case_temp_delta;  // require (case_temp <= setpoint - <delta>) to step down setpoint
    };

    uint8_t pwm_percent_min;
    uint8_t pwm_percent_max1;
    uint8_t pwm_percent_max2;

    float setpoint;  // Default Setpoint
    float setpoint_min;
    float setpoint_max;

    // PID Tunings
    //double Kp=39, Ki=1.61, Kd=0.05;  // sheet5  // 100ms, ideal for router cooling experiment
    //double Kp = 39, Ki = 1.03, Kd = 0.03; // sufficient for 360+120 radiators aggresive tunings
    uint8_t gain_p;  //39
    float gain_i;  //1.03f
    float gain_d;  //0.03f

    bool adaptive_sp;  // adapt setpoint by 0.5C within setpoint_min >= setpoint >= setpoint_max
    bool adaptive_sp_check_case_temp;
    float adaptive_sp_step_size;
    PIDStep adaptive_sp_step_down;
    PIDStep adaptive_sp_step_up;
  };
  struct SensorConfig {
    uint8_t pin;
    uint16_t beta;
    uint16_t seriesR;
    uint16_t nominalR;
    PIDConfig pid;
  };

  uint8_t config_version;

  FanConfig fan1;
  FanConfig fan2;
  FanConfig fan3;
  FanConfig fan4;
  FanConfig fan5;
  FanConfig fan6;

  SensorConfig tempSupply;
  SensorConfig tempReturn;
  SensorConfig tempCase;
  SensorConfig tempAux1;
  SensorConfig tempAux2;

  RuntimeConfig();
  RuntimeConfig(uint8_t config_version,
                FanConfig fan1,
                FanConfig fan2,
                FanConfig fan3,
                FanConfig fan4,
                FanConfig fan5,
                FanConfig fan6,
                SensorConfig tempSupply,
                SensorConfig tempReturn,
                SensorConfig tempCase,
                SensorConfig tempAux1,
                SensorConfig tempAux2);

  int to_bytes(byte *bytes, const size_t &len);

  static RuntimeConfig parse_bytes(const byte bytes[], const size_t &len);
};


/**
   Version 1 of RuntimeConfig.  Compressed, fe. some floats stored as uint8_t (by multiplying them by 100).
*/
struct __RuntimeConfig_v1 {
  struct __TableConfig_v1 {
    uint16_t temp_pct_table[10][2];

    RuntimeConfig::TableConfig decompress() const;

    static __TableConfig_v1 compress(RuntimeConfig::TableConfig in);
  };
  struct __FanConfig_v1 {
    uint8_t pinPWM;
    uint8_t pinRPM;
    CONTROL_MODE mode;
    CONTROL_SOURCE source;
    uint8_t ratio;
    __TableConfig_v1 tbl;

    RuntimeConfig::FanConfig decompress() const;

    static __FanConfig_v1 compress(RuntimeConfig::FanConfig in);
  };
  struct __PIDConfig_v1 {
    struct __PIDStep_v1 {
      uint8_t pct;
      uint16_t delay;
      uint8_t case_temp_delta;

      RuntimeConfig::PIDConfig::PIDStep decompress() const;

      static __PIDStep_v1 compress(RuntimeConfig::PIDConfig::PIDStep in);
    };

    uint8_t pwm_percent_min;
    uint8_t pwm_percent_max1;
    uint8_t pwm_percent_max2;

    uint8_t setpoint;
    uint8_t setpoint_min;
    uint8_t setpoint_max;

    uint8_t gain_p;
    uint8_t gain_i;
    uint8_t gain_d;

    bool adaptive_sp;
    bool adaptive_sp_check_case_temp;
    uint8_t adaptive_sp_step_size;
    __PIDStep_v1 adaptive_sp_step_down;
    __PIDStep_v1 adaptive_sp_step_up;

    RuntimeConfig::PIDConfig decompress() const;

    static __PIDConfig_v1 compress(RuntimeConfig::PIDConfig in);
  };
  struct __SensorConfig_v1 {
    uint8_t pin;
    uint16_t beta;
    uint16_t seriesR;
    uint8_t nominalR;
    __PIDConfig_v1 pid;

    RuntimeConfig::SensorConfig decompress() const;

    static __SensorConfig_v1 compress(RuntimeConfig::SensorConfig in);
  };

  uint8_t config_version;

  __FanConfig_v1 fan1;
  __FanConfig_v1 fan2;
  __FanConfig_v1 fan3;
  __FanConfig_v1 fan4;
  __FanConfig_v1 fan5;
  __FanConfig_v1 fan6;

  __SensorConfig_v1 tempSupply;
  __SensorConfig_v1 tempReturn;
  __SensorConfig_v1 tempCase;
  __SensorConfig_v1 tempAux1;
  __SensorConfig_v1 tempAux2;

  RuntimeConfig decompress() const;

  static __RuntimeConfig_v1 compress(RuntimeConfig in);
};


#endif //TFC_RUNTIMECONFIG_H
