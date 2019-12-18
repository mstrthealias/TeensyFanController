//
// Created by jd on 11/26/2019.
//

#ifndef TFC_RUNTIMECONFIG_H
#define TFC_RUNTIMECONFIG_H

#include <Arduino.h>


// thermistor coefficients/defaults
#define TEMPERATURENOMINAL 25  // temp. for thermistor nominal resistance
#define DEFAULT_BCOEFFICIENT 3950  // Default beta coefficient of the thermistor (usually 3000-4000)
#define DEFAULT_SERIESRESISTOR 9377  // Default value of the 'other' resistor
#define DEFAULT_THERMISTORNOMINAL 10000  // Default thermistor nominal resistence (at nominal temp)

#define CONTROLLER_VERSION 1  // use to flag the version of configuration saved
#define CONTROLLER_KEY1 'j'  // use to flag a valid config
#define CONTROLLER_KEY2 'd'  // use to flag a valid config

#define CONFIG_POS_VERSION              0
#define CONFIG_POS_KEY1                 1
#define CONFIG_POS_KEY2                 2
#define CONFIG_POS_CONFIG               4

#define CONFIG_BYTES 127


enum CONTROL_MODE : uint8_t {
  MODE_PERCENT_TABLE,
  MODE_PID
};

enum TBL_INPUT : uint8_t {
  SENSOR_WATER_SUPPLY_TEMP,
  SENSOR_WATER_RETURN_TEMP,
  SENSOR_CASE_TEMP,
  SENSOR_AUX_TEMP,
  VIRTUAL_DELTA_TEMP,
};


/**
   Fan controller's runtime configuration struct.
*/
struct RuntimeConfig {
  struct SensorConfig {
    uint8_t pin;
    uint16_t beta;
    uint16_t seriesR;
    uint16_t nominalR;
  };
  struct FanConfig {
    uint8_t pinPWM;
    uint8_t pinRPM;
    CONTROL_MODE mode;
    float ratio;
  };
  struct PIDConfig {
    struct PIDStep {
      uint8_t pct;
      uint16_t delay;  // # of seconds fan % must be below step_down_pct to step setpoint down 0.5C
      float case_temp_delta;  // require (case_temp <= setpoint - <delta>) to step down setpoint
    };

    float setpoint = 28.0f;  // Default Setpoint
    float setpoint_min = 28.0f;
    float setpoint_max = 31.5f;

    // PID Tunings
    //double Kp=39, Ki=1.61, Kd=0.05;  // sheet5  // 100ms, ideal for router cooling experiment
    //double Kp = 39, Ki = 1.03, Kd = 0.03; // sufficient for 360+120 radiators aggresive tunings
    uint8_t gain_p = 34;  //39
    float gain_i = 0.9f;  //1.03f
    float gain_d = 0.02f;  //0.03f

    bool adaptive_sp = true;  // adapt setpoint by 0.5C within setpoint_min >= setpoint >= setpoint_max
    bool adaptive_sp_check_case_temp = true;
    float adaptive_sp_step_size = 0.5f;
    PIDStep adaptive_sp_step_down = {45, 60, 2.1f};
    PIDStep adaptive_sp_step_up = {65, 30, 0.3f};

    bool adaptive_tuning = true;  // use a more aggressive tuning factor when DeltaT > delta_t_threshold
    uint8_t adaptive_tuning_delay = 5;
    float adaptive_tuning_delta_t_threshold = 2.5f;
    float adaptive_tuning_multiplier = 1.15f;
  };
  // TODO
  struct TableConfig {
    TBL_INPUT input = VIRTUAL_DELTA_TEMP;

    uint8_t tempPercentTable[10][2] = {
      {25, 0},
      {25, 0},
      {25, 28},
      {28, 35},
      {29, 45},
      {30, 55},
      {31, 65},
      {32, 75},
      {33, 85},
      {35, 100},
    };
  };

  uint8_t config_version = CONTROLLER_VERSION;

  FanConfig fan1 = {4, 5, MODE_PID, 1};
  FanConfig fan2 = {6, 7, MODE_PID, 0.8f};
  FanConfig fan3 = {10, 11, MODE_PID, 1};
  FanConfig fan4 = {9, 8, MODE_PID, 0.8f};
  FanConfig fan5 = {3, 2, MODE_PID, 1};
  FanConfig fan6 = {22, 12, MODE_PID, 1};

  SensorConfig tempSupply = {A7, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL};
  SensorConfig tempReturn = {A6, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL};
  SensorConfig tempCase = {A4, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL};
  SensorConfig tempAux = {0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL};

  uint8_t pwm_percent_min = 24;
  uint8_t pwm_percent_max1 = 75;
  uint8_t pwm_percent_max2 = 100;

  PIDConfig pid;
  TableConfig tbl;


  int to_bytes(byte *bytes, size_t len);

  static RuntimeConfig parse_bytes(const byte bytes[], size_t len);
};


/**
   Version 1 of RuntimeConfig.  Compressed, fe. some floats stored as uint8_t (by multiplying them by 100).
*/
struct __RuntimeConfig_v1 {
  struct __SensorConfig_v1 {
    uint8_t pin;
    uint16_t beta;
    uint16_t seriesR;
    uint8_t nominalR;

    RuntimeConfig::SensorConfig decompress();

    static __SensorConfig_v1 compress(RuntimeConfig::SensorConfig in);
  };
  struct __FanConfig_v1 {
    uint8_t pinPWM;
    uint8_t pinRPM;
    CONTROL_MODE mode;
    uint8_t ratio;

    RuntimeConfig::FanConfig decompress();

    static __FanConfig_v1 compress(RuntimeConfig::FanConfig in);
  };
  struct __PIDConfig_v1 {
    struct __PIDStep_v1 {
      uint8_t pct;
      uint16_t delay;
      uint8_t case_temp_delta;

      RuntimeConfig::PIDConfig::PIDStep decompress();

      static __PIDStep_v1 compress(RuntimeConfig::PIDConfig::PIDStep in);
    };

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

    bool adaptive_tuning;
    uint8_t adaptive_tuning_delay;
    uint16_t adaptive_tuning_delta_t_threshold;
    uint8_t adaptive_tuning_multiplier;

    RuntimeConfig::PIDConfig decompress();

    static __PIDConfig_v1 compress(RuntimeConfig::PIDConfig in);
  };
  //struct __TableConfig_v1 {
  //};

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
  __SensorConfig_v1 tempAux;

  uint8_t pwm_percent_min;
  uint8_t pwm_percent_max1;
  uint8_t pwm_percent_max2;

  __PIDConfig_v1 pid;
  RuntimeConfig::TableConfig tbl;

  RuntimeConfig decompress();

  static __RuntimeConfig_v1 compress(RuntimeConfig in);
};


#endif //TFC_RUNTIMECONFIG_H
