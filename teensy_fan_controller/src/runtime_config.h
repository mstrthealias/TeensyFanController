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
#define A3 0
#define A4 0
#define A6 0
#define A7 0
#endif

#include "runtime_config_v1.pb.h"


// thermistor coefficients/defaults
#define TEMPERATURENOMINAL 25  // temp. for thermistor nominal resistance
#define DEFAULT_BCOEFFICIENT 3950  // Default beta coefficient of the thermistor (usually 3000-4000)
#define DEFAULT_SERIESRESISTOR 9377  // Default value of the 'other' resistor
#define DEFAULT_THERMISTORNOMINAL 10000  // Default thermistor nominal resistence (at nominal temp)

// core related constants
#define FAN_TBL_SIZE 10

// config related constants
#define CONTROLLER_VERSION 1  // use to flag the version of configuration saved
#define CONFIG_KEY1 0x2C  // use to flag a valid config
#define CONFIG_KEY2 0x30  // use to flag a valid config

#define CONFIG_POS_VERSION              0
#define CONFIG_POS_KEY1                 1
#define CONFIG_POS_KEY2                 2
#define CONFIG_POS_CONFIG_LENGTH        4  // spans 2 bytes
#define CONFIG_POS_CONFIG               6

#define CONFIG_BYTES 1008
#define CHUNK_SIZE 56
#define CHUNK_CNT 18


enum class CONTROL_MODE : uint8_t {
  MODE_TBL,
  MODE_PID,
  MODE_FIXED,
  MODE_OFF
};

tfcproto_ControlMode CONTROL_MODE_to_proto(CONTROL_MODE in);

CONTROL_MODE CONTROL_MODE_from_proto(const tfcproto_ControlMode &in);


enum class CONTROL_SOURCE : uint8_t {
  SENSOR_WATER_SUPPLY_TEMP,
  SENSOR_WATER_RETURN_TEMP,
  SENSOR_CASE_TEMP,
  SENSOR_AUX1_TEMP,
  SENSOR_AUX2_TEMP,
  VIRTUAL_DELTA_TEMP,
};

tfcproto_ControlSource CONTROL_SOURCE_to_proto(CONTROL_SOURCE in);

CONTROL_SOURCE CONTROL_SOURCE_from_proto(const tfcproto_ControlSource &in);


/**
   Fan controller's runtime configuration struct.
*/
struct RuntimeConfig {
  struct TableConfig {
    float temp_pct_table[FAN_TBL_SIZE][2];

    tfcproto_TableConfigV1 toProto() const;

    static TableConfig from_proto(tfcproto_TableConfigV1 &in);
  };
  struct FanConfig {
    uint8_t pinPWM;
    uint8_t pinRPM;
    CONTROL_MODE mode;
    CONTROL_SOURCE source;
    float ratio;
    TableConfig tbl;

    tfcproto_FanConfigV1 toProto() const;

    static FanConfig from_proto(tfcproto_FanConfigV1 &in);
  };
  struct PIDConfig {
    struct PIDStep {
      uint8_t pct;
      uint16_t delay;  // # of seconds fan % must be below step_down_pct to step setpoint down 0.5C
      float case_temp_delta;  // require (case_temp <= setpoint - <delta>) to step down setpoint

      tfcproto_PIDStepV1 toProto() const;

      static PIDStep from_proto(tfcproto_PIDStepV1 &in);
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

    tfcproto_PIDConfigV1 toProto() const;

    static PIDConfig from_proto(tfcproto_PIDConfigV1 &in);
  };
  struct SensorConfig {
    uint8_t pin;
    uint16_t beta;
    uint16_t seriesR;
    uint16_t nominalR;
    PIDConfig pid;

    tfcproto_SensorConfigV1 toProto() const;

    static SensorConfig from_proto(tfcproto_SensorConfigV1 &in);
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

  int toBytes(byte *bytes, const uint16_t len) const;
  tfcproto_RuntimeConfigV1 toProto() const;

  static RuntimeConfig parse_bytes(const byte bytes[], const uint16_t len);
  static RuntimeConfig from_proto(tfcproto_RuntimeConfigV1 &config);
};


#endif //TFC_RUNTIMECONFIG_H
