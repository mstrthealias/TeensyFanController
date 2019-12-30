//
// Created by jd on 11/26/2019.
//

#include "runtime_config.h"
#include <memory>
#include <cstring>


RuntimeConfig::RuntimeConfig() :
    config_version{CONTROLLER_VERSION},
    fan1{4, 5, MODE_PID, 1},
    fan2{6, 7, MODE_PID, 0.8f},
    fan3{10, 11, MODE_PID, 1},
    fan4{9, 8, MODE_PID, 0.8f},
    fan5{3, 2, MODE_PID, 1},
    fan6{22, 12, MODE_PID, 1},
    tempSupply{A7, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL},
    tempReturn{A6, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL},
    tempCase{A4, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL},
    tempAux{0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL},
    pwm_percent_min{24},
    pwm_percent_max1{75},
    pwm_percent_max2{100},
    pid{
          28.0f,  // Default Setpoint
          28.0f,
          31.5f,
          34,  //39
          0.9f,  //1.03f
          0.02f,  //0.03f
          true,  // adapt setpoint by 0.5C within setpoint_min >= setpoint >= setpoint_max
          true,
          0.5f,
          {45, 60, 2.1f},
          {65, 30, 1.8f},
          true,  // use a more aggressive tuning factor when DeltaT > delta_t_threshold
          5,
          2.5f,
          1.15f
      },
      tbl{
          VIRTUAL_DELTA_TEMP,
          {
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
           }
      }
{
}

RuntimeConfig::RuntimeConfig(uint8_t config_version,
        FanConfig fan1,
        FanConfig fan2,
        FanConfig fan3,
        FanConfig fan4,
        FanConfig fan5,
        FanConfig fan6,
        SensorConfig tempSupply,
        SensorConfig tempReturn,
        SensorConfig tempCase,
        SensorConfig tempAux,
        uint8_t pwm_percent_min,
        uint8_t pwm_percent_max1,
        uint8_t pwm_percent_max2,
        PIDConfig pid,
        TableConfig tbl)
    : config_version{config_version},
      fan1{fan1}, fan2{fan2}, fan3{fan3}, fan4{fan4}, fan5{fan5}, fan6{fan6},
      tempSupply{tempSupply}, tempReturn{tempReturn}, tempCase{tempCase}, tempAux{tempAux},
      pwm_percent_min{pwm_percent_min}, pwm_percent_max1{pwm_percent_max1}, pwm_percent_max2{pwm_percent_max2},
      pid{pid}, tbl{tbl}
{
}

int RuntimeConfig::to_bytes(byte *bytes, const size_t &len) {
  if (len < CONFIG_BYTES) {
#ifdef ARDUINO
    Serial.println("Logic Error: 127 bytes needed for program configuration");
#endif
    return -1;
  }

  memset(bytes, '\0', len);  // null all bytes

  bytes[CONFIG_POS_VERSION] = CONTROLLER_VERSION;
  bytes[CONFIG_POS_KEY1] = CONTROLLER_KEY1;
  bytes[CONFIG_POS_KEY2] = CONTROLLER_KEY2;

  __RuntimeConfig_v1 config = __RuntimeConfig_v1::compress(*this);
  memcpy((bytes + CONFIG_POS_CONFIG), &config, sizeof(__RuntimeConfig_v1));

  return 0;
}

/*static */RuntimeConfig RuntimeConfig::parse_bytes(const byte bytes[], const size_t &len) {
  if (len < CONFIG_BYTES) {
#ifdef ARDUINO
    Serial.println("Logic Error: 127 bytes needed to parse program configuration, defaults returned");
#endif
    return RuntimeConfig();  // Return defaults
  }
  else if (static_cast<uint8_t>(bytes[CONFIG_POS_VERSION]) > CONTROLLER_VERSION || static_cast<uint8_t>(bytes[CONFIG_POS_VERSION]) < 1 || static_cast<char>(bytes[CONFIG_POS_KEY1]) != CONTROLLER_KEY1 || static_cast<char>(bytes[CONFIG_POS_KEY2]) != CONTROLLER_KEY2) {
    return RuntimeConfig();  // Return defaults
  }

  __RuntimeConfig_v1 _config;
  memcpy(&_config, (bytes + CONFIG_POS_CONFIG), sizeof(__RuntimeConfig_v1));

  return _config.decompress();
}



RuntimeConfig __RuntimeConfig_v1::decompress() {
  return {
    config_version,
    fan1.decompress(),
    fan2.decompress(),
    fan3.decompress(),
    fan4.decompress(),
    fan5.decompress(),
    fan6.decompress(),
    tempSupply.decompress(),
    tempReturn.decompress(),
    tempCase.decompress(),
    tempAux.decompress(),
    pwm_percent_min,
    pwm_percent_max1,
    pwm_percent_max2,
    pid.decompress(),
    tbl
  };
}

/*static */__RuntimeConfig_v1 __RuntimeConfig_v1::compress(RuntimeConfig in) {
  return {
    in.config_version,
    __FanConfig_v1::compress(in.fan1),
    __FanConfig_v1::compress(in.fan2),
    __FanConfig_v1::compress(in.fan3),
    __FanConfig_v1::compress(in.fan4),
    __FanConfig_v1::compress(in.fan5),
    __FanConfig_v1::compress(in.fan6),
    __SensorConfig_v1::compress(in.tempSupply),
    __SensorConfig_v1::compress(in.tempReturn),
    __SensorConfig_v1::compress(in.tempCase),
    __SensorConfig_v1::compress(in.tempAux),
    in.pwm_percent_min,
    in.pwm_percent_max1,
    in.pwm_percent_max2,
    __PIDConfig_v1::compress(in.pid),
    in.tbl
  };
}


RuntimeConfig::SensorConfig __RuntimeConfig_v1::__SensorConfig_v1::decompress() {
  return {
    pin,
    beta,
    seriesR,
    static_cast<uint16_t>(nominalR * 1000)
  };
}

/*static */__RuntimeConfig_v1::__SensorConfig_v1 __RuntimeConfig_v1::__SensorConfig_v1::compress(RuntimeConfig::SensorConfig in) {
  return {
    in.pin,
    in.beta,
    in.seriesR,
    static_cast<uint8_t>(in.nominalR / 1000)
  };
}


RuntimeConfig::FanConfig __RuntimeConfig_v1::__FanConfig_v1::decompress() {
  return {
    pinPWM,
    pinRPM,
    mode,
    static_cast<float>(ratio / 100.0)
  };
}

/*static */__RuntimeConfig_v1::__FanConfig_v1 __RuntimeConfig_v1::__FanConfig_v1::compress(RuntimeConfig::FanConfig in) {
  return {
    in.pinPWM,
    in.pinRPM,
    in.mode,
    static_cast<uint8_t>(in.ratio * 100)
  };
}


RuntimeConfig::PIDConfig __RuntimeConfig_v1::__PIDConfig_v1::decompress() {
  return {
    static_cast<float>(setpoint / 4.0),
    static_cast<float>(setpoint_min / 4.0),
    static_cast<float>(setpoint_max / 4.0),
    gain_p,
    static_cast<float>(gain_i / 100.0),
    static_cast<float>(gain_d / 100.0),
    adaptive_sp,
    adaptive_sp_check_case_temp,
    static_cast<float>(adaptive_sp_step_size / 50.0),
    adaptive_sp_step_down.decompress(),
    adaptive_sp_step_up.decompress(),
    adaptive_tuning,
    adaptive_tuning_delay,
    static_cast<float>(adaptive_tuning_delta_t_threshold / 100.0),
    static_cast<float>(adaptive_tuning_multiplier / 100.0)
  };
}

/*static */__RuntimeConfig_v1::__PIDConfig_v1 __RuntimeConfig_v1::__PIDConfig_v1::compress(RuntimeConfig::PIDConfig in) {
  return {
    static_cast<uint8_t>(in.setpoint * 4),
    static_cast<uint8_t>(in.setpoint_min * 4),
    static_cast<uint8_t>(in.setpoint_max * 4),
    in.gain_p,
    static_cast<uint8_t>(in.gain_i * 100),
    static_cast<uint8_t>(in.gain_d * 100),
    in.adaptive_sp,
    in.adaptive_sp_check_case_temp,
    static_cast<uint8_t>(in.adaptive_sp_step_size * 50),
    __PIDStep_v1::compress(in.adaptive_sp_step_down),
    __PIDStep_v1::compress(in.adaptive_sp_step_up),
    in.adaptive_tuning,
    in.adaptive_tuning_delay,
    static_cast<uint16_t>(in.adaptive_tuning_delta_t_threshold * 100),
    static_cast<uint8_t>(in.adaptive_tuning_multiplier * 100)
  };
}


RuntimeConfig::PIDConfig::PIDStep __RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1::decompress() {
  return {
    pct,
    delay,
    static_cast<float>(case_temp_delta / 50.0)
  };
}

/*static */__RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1 __RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1::compress(RuntimeConfig::PIDConfig::PIDStep in) {
  return {
    in.pct,
    in.delay,
    static_cast<uint8_t>(in.case_temp_delta * 50)
  };
}
