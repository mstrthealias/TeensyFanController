//
// Created by jd on 11/26/2019.
//

#include "temp_controller.h"


TempController::TempController(const RuntimeConfig &config, uint16_t samplePeriod, const SensorData &supplyTemp, const SensorData &returnTemp, const SensorData &caseTemp, const SensorData &auxTemp, const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6, const void (*setupHardware)(), const void (*saveConfig)()) :
  config(config), _pid_setpoint(config.pid.setpoint), pid(&_pid_input, &_pid_pct, &_pid_setpoint, config.pid.gain_p, config.pid.gain_i, config.pid.gain_d, REVERSE),
  supplyTemp(supplyTemp), returnTemp(returnTemp), caseTemp(caseTemp), auxTemp(auxTemp), fan1(fan1), fan2(fan2), fan3(fan3), fan4(fan4), fan5(fan5), fan6(fan6),
  setupHardware(setupHardware), saveConfig(saveConfig)
{
  // turn the PID on
  pid.SetMode(AUTOMATIC);
  if (_pid_setpoint >= config.pid.setpoint_max)
    pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max2);
  else
    pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max1);
  pid.SetSampleTime(samplePeriod);
}

void TempController::configChanged() {
  saveConfig();  // save config into EEPROM

  // update local variables used for fan % calcuations:
  _pid_setpoint = config.pid.setpoint;
  // TODO set gains
  if (_pid_setpoint >= config.pid.setpoint_max)
    pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max2);
  else
    pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max1);

  setupHardware();  // setup pin muxing
}

float TempController::getTempSetpoint() {
  return _pid_setpoint;
}

float TempController::getFanPercentPID() {
  return _pid_pct;
}

float TempController::getFanPercentTbl() {
  return _tbl_pct;
}

float TempController::getDeltaT() {
  return returnTemp.val - supplyTemp.val;
}

float TempController::getTableInputTemp() {
  switch (config.tbl.input) {
    case SENSOR_WATER_SUPPLY_TEMP:
      return supplyTemp.val;
    case SENSOR_WATER_RETURN_TEMP:
      return returnTemp.val;
    case SENSOR_CASE_TEMP:
      return caseTemp.val;
    case SENSOR_AUX_TEMP:
      return auxTemp.val;
    case VIRTUAL_DELTA_TEMP:
      return getDeltaT();
  }
  return 0;
}

float TempController::sample(CONTROL_MODE mode, float sample) {
  if (mode == MODE_PID) {
    _pid_input = sample;
    // Calculates Fan % using PID controller
    pid.Compute();
    float pct = _pid_pct;
    if (config.pid.adaptive_sp) {
      // adapt set-point when fan% below or above thresholds
      bool changed = false;
      bool withinCaseTempLimit = true;
      if (pct <= config.pid.adaptive_sp_step_down.pct) {
        lastSincePercentsAboveLimit = 0;
        if (lastSincePercentsBelowLimit == 0)
          lastSincePercentsBelowLimit = millis();
        if (millis() - lastSincePercentsBelowLimit > config.pid.adaptive_sp_step_down.delay * 1000) {
          // (temp has been below limit for the required duration)
          if (config.pid.adaptive_sp_check_case_temp && caseTemp.cfg.pin) {
            // require (case_temp <= setpoint - <delta>) to step down setpoint
            if (caseTemp.val > _pid_setpoint - config.pid.adaptive_sp_step_down.case_temp_delta)
              withinCaseTempLimit = false;
          }
          if (withinCaseTempLimit && _pid_setpoint - config.pid.adaptive_sp_step_size >= config.pid.setpoint_min) {
            // decrement setpoint by step size
            _pid_setpoint -= config.pid.adaptive_sp_step_size;
            lastSincePercentsBelowLimit = 0;
            changed = true;
          }
        }
      }
      else if (pct >= config.pid.adaptive_sp_step_up.pct) {
        lastSincePercentsBelowLimit = 0;
        if (lastSincePercentsAboveLimit == 0)
          lastSincePercentsAboveLimit = millis();
        if (millis() - lastSincePercentsAboveLimit > config.pid.adaptive_sp_step_up.delay * 1000) {
          // (temp has been above limit for the required duration)
          if (config.pid.adaptive_sp_check_case_temp && caseTemp.cfg.pin) {
            // require (case_temp >= setpoint - <delta>) to step up setpoint
            if (caseTemp.val < _pid_setpoint - config.pid.adaptive_sp_step_up.case_temp_delta)
              withinCaseTempLimit = false;
          }
          if (withinCaseTempLimit && _pid_setpoint + config.pid.adaptive_sp_step_size <= config.pid.setpoint_max) {
            // increment setpoint by step size
            _pid_setpoint += config.pid.adaptive_sp_step_size;
            lastSincePercentsAboveLimit = 0;
            changed = true;
          }
        }
      }
      else {
        lastSincePercentsAboveLimit = 0;
        lastSincePercentsBelowLimit = 0;
      }
      if (changed || !withinCaseTempLimit) {
        // update fan % limit
        if (!withinCaseTempLimit || _pid_setpoint >= config.pid.setpoint_max)
          pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max2);
        else
          pid.SetOutputLimits(config.pwm_percent_min, config.pwm_percent_max1);
      }
    }
    return pct;
  }
  else if (mode == MODE_PERCENT_TABLE) {
    // Calculates Fan % using only tempPercentTable
    uint8_t i;
    uint8_t lastTemp = 0, lastPct = 0,
            curTemp, curPct;
    for (i = 0; i < 9; i++) {
      curTemp = config.tbl.tempPercentTable[i][0];
      curPct = config.tbl.tempPercentTable[i][1];
      if (sample <= curTemp) {  // use this entry
        if (curPct >= 100) {
          return _tbl_pct = curPct;  // no > than 100
        }
        else if (i > 0) {  // calc % between lastEntry and this entry
          return _tbl_pct = (1 - ((curTemp - sample) / (curTemp - lastTemp))) * (curPct - lastPct) + lastPct;
        }
        else {  // use % in table
          return _tbl_pct = curPct;
        }
      }
      lastPct = curPct;
      lastTemp = curTemp;
    }
    return _tbl_pct = curPct;
  }
  return 0;
}
