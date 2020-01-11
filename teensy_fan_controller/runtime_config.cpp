//
// Created by jd on 11/26/2019.
//

#include <memory>
#include <cstring>

#include "runtime_config.h"


static_assert(CONFIG_BYTES >= sizeof(__RuntimeConfig_v1) + 4, "CONFIG_BYTES must be at least sizeof(__RuntimeConfig_v1) + 4");


#define DEFAULT_PERCENT_TABLE {{25, 0}, {25, 0}, {25, 28}, {28, 35}, {29, 45}, {30, 55}, {31, 65}, {32, 75}, {33, 85}, {35, 100}}

#define DEFAULT_PID {\
    24,\
    75,\
    100,\
    28.0f,\
    28.0f,\
    31.5f,\
    34,\
    0.9f,\
    0.02f,\
    true,\
    true,\
    0.5f,\
    {45, 60, 2.1f},\
    {65, 30, 1.8f}\
  }


RuntimeConfig::RuntimeConfig()
  : config_version{CONTROLLER_VERSION},
    fan1{4, 5, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
    fan2{6, 7, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 0.8f, {DEFAULT_PERCENT_TABLE}},
    fan3{10, 11, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
    fan4{9, 8, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 0.8f, {DEFAULT_PERCENT_TABLE}},
    fan5{3, 2, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
    fan6{22, 12, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
    tempSupply{A7, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
    tempReturn{A6, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
    tempCase{A4, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
    tempAux1{0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
    tempAux2{0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID}
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
                             SensorConfig tempAux1,
                             SensorConfig tempAux2)
  : config_version{config_version},
    fan1{fan1}, fan2{fan2}, fan3{fan3}, fan4{fan4}, fan5{fan5}, fan6{fan6},
    tempSupply{tempSupply}, tempReturn{tempReturn}, tempCase{tempCase}, tempAux1{tempAux1}, tempAux2{tempAux2}
{
}

int RuntimeConfig::to_bytes(byte *bytes, const size_t &len)
{
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

/*static */RuntimeConfig RuntimeConfig::parse_bytes(const byte bytes[], const size_t &len)
{
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



RuntimeConfig __RuntimeConfig_v1::decompress() const
{
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
    tempAux1.decompress(),
    tempAux2.decompress()
  };
}

/*static */__RuntimeConfig_v1 __RuntimeConfig_v1::compress(RuntimeConfig in)
{
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
    __SensorConfig_v1::compress(in.tempAux1),
    __SensorConfig_v1::compress(in.tempAux2)
  };
}


RuntimeConfig::SensorConfig __RuntimeConfig_v1::__SensorConfig_v1::decompress() const
{
  return {
    pin,
    beta,
    seriesR,
    static_cast<uint16_t>(nominalR * 1000),
    pid.decompress()
  };
}

/*static */__RuntimeConfig_v1::__SensorConfig_v1 __RuntimeConfig_v1::__SensorConfig_v1::compress(RuntimeConfig::SensorConfig in)
{
  return {
    in.pin,
    in.beta,
    in.seriesR,
    static_cast<uint8_t>(in.nominalR / 1000),
    __PIDConfig_v1::compress(in.pid)
  };
}


RuntimeConfig::FanConfig __RuntimeConfig_v1::__FanConfig_v1::decompress() const
{
  return {
    pinPWM,
    pinRPM,
    mode,
    source,
    static_cast<float>(ratio / 100.0),
    tbl
  };
}

/*static */__RuntimeConfig_v1::__FanConfig_v1 __RuntimeConfig_v1::__FanConfig_v1::compress(RuntimeConfig::FanConfig in)
{
  return {
    in.pinPWM,
    in.pinRPM,
    in.mode,
    in.source,
    static_cast<uint8_t>(in.ratio * 100),
    in.tbl
  };
}


RuntimeConfig::PIDConfig __RuntimeConfig_v1::__PIDConfig_v1::decompress() const
{
  return {
    pwm_percent_min,
    pwm_percent_max1,
    pwm_percent_max2,
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
    adaptive_sp_step_up.decompress()
  };
}

/*static */__RuntimeConfig_v1::__PIDConfig_v1 __RuntimeConfig_v1::__PIDConfig_v1::compress(RuntimeConfig::PIDConfig in)
{
  return {
    in.pwm_percent_min,
    in.pwm_percent_max1,
    in.pwm_percent_max2,
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
    __PIDStep_v1::compress(in.adaptive_sp_step_up)
  };
}


RuntimeConfig::PIDConfig::PIDStep __RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1::decompress() const
{
  return {
    pct,
    delay,
    static_cast<float>(case_temp_delta / 50.0)
  };
}

/*static */__RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1 __RuntimeConfig_v1::__PIDConfig_v1::__PIDStep_v1::compress(RuntimeConfig::PIDConfig::PIDStep in)
{
  return {
    in.pct,
    in.delay,
    static_cast<uint8_t>(in.case_temp_delta * 50)
  };
}
