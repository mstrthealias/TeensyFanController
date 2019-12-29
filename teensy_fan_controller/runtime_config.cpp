//
// Created by jd on 11/26/2019.
//

#include "runtime_config.h"
#include <memory>


int RuntimeConfig::to_bytes(byte *bytes, const size_t &len) {
  if (len < CONFIG_BYTES) {
    Serial.println("Logic Error: 127 bytes needed for program configuration");
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
