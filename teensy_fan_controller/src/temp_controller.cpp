//
// Created by jd on 11/26/2019.
//

#include "temp_controller.h"


TempController::PIDController::PIDController(const RuntimeConfig::PIDConfig &pidCfg)
    : pidCfg(pidCfg), setpoint(pidCfg.setpoint), in(0), pct(0),
      pid(&in, &pct, &setpoint, pidCfg.gain_p, pidCfg.gain_i, pidCfg.gain_d, PID::REVERSE)
{
  // configure PID
  pid.SetSampleTime(SAMPLE_PERIOD);
  if (setpoint >= pidCfg.setpoint_max)
    pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max2);
  else
    pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max1);
  pid.SetMode(PID::AUTOMATIC);  // start PID
}

uint8_t TempController::PIDController::sample(const float &sample, const SensorData &caseTemp)
{
  in = sample;
  pid.Compute();  // Calculate Fan % using PID controller

  if (pidCfg.adaptive_sp) {
    // adapt set-point when fan% below or above thresholds
    bool changed = false;
    bool withinCaseTempLimit = true;
    if (pct <= pidCfg.adaptive_sp_step_down.pct) {
      lastSincePercentsAboveLimit = 0;
      if (lastSincePercentsBelowLimit == 0)
        lastSincePercentsBelowLimit = millis();
      if (millis() - lastSincePercentsBelowLimit > pidCfg.adaptive_sp_step_down.delay * 1000) {
        // (temp has been below limit for the required duration)
        if (pidCfg.adaptive_sp_check_case_temp && caseTemp.cfg.pin) {
          // require (case_temp <= setpoint - <delta>) to step down setpoint
          if (caseTemp.val > setpoint - pidCfg.adaptive_sp_step_down.case_temp_delta)
            withinCaseTempLimit = false;
        }
        if (withinCaseTempLimit && setpoint - pidCfg.adaptive_sp_step_size >= pidCfg.setpoint_min) {
          // decrement setpoint by step size
          setpoint -= pidCfg.adaptive_sp_step_size;
          lastSincePercentsBelowLimit = 0;
          changed = true;
        }
      }
    }
    else if (pct >= pidCfg.adaptive_sp_step_up.pct) {
      lastSincePercentsBelowLimit = 0;
      if (lastSincePercentsAboveLimit == 0)
        lastSincePercentsAboveLimit = millis();
      if (millis() - lastSincePercentsAboveLimit > pidCfg.adaptive_sp_step_up.delay * 1000) {
        // (temp has been above limit for the required duration)
        if (pidCfg.adaptive_sp_check_case_temp && caseTemp.cfg.pin) {
          // require (case_temp >= setpoint - <delta>) to step up setpoint
          if (caseTemp.val < setpoint - pidCfg.adaptive_sp_step_up.case_temp_delta)
            withinCaseTempLimit = false;
        }
        if (withinCaseTempLimit && setpoint + pidCfg.adaptive_sp_step_size <= pidCfg.setpoint_max) {
          // increment setpoint by step size
          setpoint += pidCfg.adaptive_sp_step_size;
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
      if (!withinCaseTempLimit || setpoint >= pidCfg.setpoint_max)
        pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max2);
      else
        pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max1);
    }
  }
  return pct;
}

const float &TempController::PIDController::getSetpoint() const
{
  return setpoint;
}


TempController::ControlData::ControlData()
    : sample(nullptr),
      pidCtrl(nullptr),
      fans{{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
      mode(CONTROL_MODE::MODE_OFF), source(static_cast<uint8_t>(CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP)),
      label("")
{}

TempController::ControlData::~ControlData()
{
  if (pidCtrl != nullptr) {
    delete pidCtrl;
    pidCtrl = nullptr;
  }
}

void TempController::ControlData::resetPIDCtrl()
{
  if (this->pidCtrl != nullptr) {
    delete this->pidCtrl;
    this->pidCtrl = nullptr;
  }
}

void TempController::ControlData::reset()
{
  this->resetPIDCtrl();

  this->sample = nullptr;
  this->pidCtrl = nullptr;
  this->fans = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};
  this->mode = CONTROL_MODE::MODE_OFF;
  this->source = static_cast<uint8_t>(CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP);
  this->pct = 0;
  this->label = "";
}

void TempController::ControlData::setPctTable(float *const sample, const CONTROL_MODE mode, const CONTROL_SOURCE source, const String &label)
{
  this->resetPIDCtrl();

  this->sample = sample;
  this->pidCtrl = nullptr;
  this->fans = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};
  this->mode = mode;
  this->source = static_cast<uint8_t>(source);
  this->pct = 0;
  this->label = label;
}

void TempController::ControlData::setPID(SensorData *const pidSensor, const CONTROL_MODE mode, const CONTROL_SOURCE source, const String &label)
{
  this->resetPIDCtrl();

  this->sample = &pidSensor->val;
  this->pidCtrl = new PIDController(pidSensor->cfg.pid);
  this->fans = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};
  this->mode = mode;
  this->source = static_cast<uint8_t>(source);
  this->pct = 0;
  this->label = label;
}

void TempController::ControlData::setFixed(float *const sample, const CONTROL_MODE mode, const uint8_t source, const uint8_t pct, const String &label)
{
  this->resetPIDCtrl();

  this->sample = sample;
  this->pidCtrl = nullptr;
  this->fans = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};
  this->mode = mode;
  this->source = source;
  this->pct = pct;
  this->label = label;
}

uint8_t TempController::ControlData::doPID(const SensorData &caseTemp)
{
  if (pidCtrl != nullptr)
    return pct = static_cast<uint8_t>(round(pidCtrl->sample(*sample, caseTemp)));
  else
    return pct = 0;
}

uint8_t TempController::ControlData::doTbl(const auto &tempPctTbl)
{
  // Calculates Fan % using only tempPercentTable
  const float &reading = *sample;
  float lastTemp = 0,
        curTemp;
  uint8_t lastPct = 0,
          curPct, i;
  for (i = 0; i < FAN_TBL_SIZE; i++) {
    curTemp = tempPctTbl[i][0];
    curPct = tempPctTbl[i][1];
    if (reading <= curTemp) {  // use this entry
      if (i > 0) {  // calc % between lastEntry and this entry
        return pct = (1 - ((curTemp - reading) / (curTemp - lastTemp))) * (curPct - lastPct) + lastPct;
      }
      else if (curPct >= 100) {
        return pct = 100;
      }
      else {  // use % in table
        return pct = curPct;
      }
    }
    lastPct = curPct;
    lastTemp = curTemp;
  }
  return pct = curPct;
}

uint8_t TempController::ControlData::doFixed()
{
  return pct;  // pct is set after construct for fixed control
}

uint8_t TempController::ControlData::getFanCount() const
{
  uint8_t i, cnt = 0;
  for (i = 0; i < FAN_CNT; i++) {
    if (fans[i] != nullptr) {
      cnt++;
    }
    else {
      break;
    }
  }
  return cnt;
}


#define MK_CONTROL_LABEL(sLbl, modeLbl) String(modeLbl) + "[" + sLbl + "]"

TempController::TempController(RuntimeConfig &config, SensorData &supplyTemp, SensorData &returnTemp, SensorData &caseTemp, SensorData &aux1Temp,
                               SensorData &aux2Temp, const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6,
                               void (*const setupHardware)(), void (*const saveConfig)()) :
    config(config),
    supplyTemp(supplyTemp), returnTemp(returnTemp), caseTemp(caseTemp), aux1Temp(aux1Temp), aux2Temp(aux2Temp),
    fans{&fan1, &fan2, &fan3, &fan4, &fan5, &fan6},
    controlModes(),
    deltaT{0},
    setupHardware(setupHardware), saveConfig(saveConfig)
{
}

TempController::ControlData &TempController::findOrCreateControlMode(CONTROL_MODE mode, uint8_t source, uint8_t i)
{
  for (TempController::ControlData &controlMode : controlModes) {
    if (controlMode.mode != CONTROL_MODE::MODE_OFF) {
      if (controlMode.mode == mode && source == controlMode.source) {
        return controlMode;
      }
    }
    else {
      // update/return first unused control mode
      if (mode == CONTROL_MODE::MODE_PID) {
        CONTROL_SOURCE src = static_cast<CONTROL_SOURCE>(source);
        switch (src) {
          case CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP:
            controlMode.setPID(&supplyTemp, mode, src, MK_CONTROL_LABEL(supplyTemp.lbl, "PID"));
            break;
          case CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP:
            controlMode.setPID(&returnTemp, mode, src, MK_CONTROL_LABEL(returnTemp.lbl, "PID"));
            break;
          case CONTROL_SOURCE::SENSOR_CASE_TEMP:
            controlMode.setPID(&caseTemp, mode, src, MK_CONTROL_LABEL(caseTemp.lbl, "PID"));
            break;
          case CONTROL_SOURCE::SENSOR_AUX1_TEMP:
            controlMode.setPID(&aux1Temp, mode, src, MK_CONTROL_LABEL(aux1Temp.lbl, "PID"));
            break;
          case CONTROL_SOURCE::SENSOR_AUX2_TEMP:
            controlMode.setPID(&aux2Temp, mode, src, MK_CONTROL_LABEL(aux2Temp.lbl, "PID"));
            break;
          case CONTROL_SOURCE::VIRTUAL_DELTA_TEMP:  // PID on virtual temp is not supported
            break;
        }
      }
      else if (mode == CONTROL_MODE::MODE_TBL) {
        CONTROL_SOURCE src = static_cast<CONTROL_SOURCE>(source);
        switch (static_cast<CONTROL_SOURCE>(src)) {
          case CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP:
            controlMode.setPctTable(&supplyTemp.val, mode, src, MK_CONTROL_LABEL(supplyTemp.lbl, "%-table"));
            break;
          case CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP:
            controlMode.setPctTable(&returnTemp.val, mode, src, MK_CONTROL_LABEL(returnTemp.lbl, "%-table"));
            break;
          case CONTROL_SOURCE::SENSOR_CASE_TEMP:
            controlMode.setPctTable(&caseTemp.val, mode, src, MK_CONTROL_LABEL(caseTemp.lbl, "%-table"));
            break;
          case CONTROL_SOURCE::SENSOR_AUX1_TEMP:
            controlMode.setPctTable(&aux1Temp.val, mode, src, MK_CONTROL_LABEL(aux1Temp.lbl, "%-table"));
            break;
          case CONTROL_SOURCE::SENSOR_AUX2_TEMP:
            controlMode.setPctTable(&aux2Temp.val, mode, src, MK_CONTROL_LABEL(aux2Temp.lbl, "%-table"));
            break;
          case CONTROL_SOURCE::VIRTUAL_DELTA_TEMP:
            controlMode.setPctTable(&deltaT, mode, src, MK_CONTROL_LABEL("DeltaT", "%-table"));
            break;
        }
      }
      else if (mode == CONTROL_MODE::MODE_FIXED) {
        // Note: fixed % fans pass deltaT to sample, although it is unused
        controlMode.setFixed(&deltaT, mode, source, source, "Fixed");
      }
      return controlMode;
    }
  }
  Serial.println("ERROR: returning last used control mode");
  return controlModes[controlModes.size() - 1];
}

void TempController::resetControlModes()
{
  for (auto &controlMode : controlModes) {
    controlMode.reset();
  }
}

void TempController::configChanged(bool doSave)
{
  if (doSave)
    saveConfig();  // save config into EEPROM

  // clear/rebuild control schemes (unique by control mode + sensor source)
  resetControlModes();

  uint8_t i, j;
  for (i = 0; i < FAN_CNT; i++) {
    if (fans[i]->cfg.mode == CONTROL_MODE::MODE_OFF)
      continue;

    // find or create matching ControlData
    ControlData &controlMode = findOrCreateControlMode(fans[i]->cfg.mode, fans[i]->cfg.mode == CONTROL_MODE::MODE_FIXED ? static_cast<uint8_t>(fans[i]->cfg.ratio * 100)
                                                                                                                        : static_cast<uint8_t>(fans[i]->cfg.source), i);

    // add this fan to fans array
    auto &trackedFans = controlMode.fans;
    for (j = 0; j < FAN_CNT; j++) {
      if (trackedFans[j] == nullptr) {
        trackedFans[j] = fans[i];
        break;
      }
    }
  }

//  for (TempController::ControlData &controlMode : controlModes) {
//    Serial.print("ControlMode mode ");
//    Serial.print(static_cast<uint8_t>(controlMode.mode));
//    Serial.print("; source ");
//    Serial.print(controlMode.source);
//    Serial.print("; num fans ");
//    Serial.println(controlMode.getFanCount());
//  }
//
  setupHardware();  // setup pin muxing
}

float TempController::getPIDSupplyTempSetpoint() const
{
  for (const TempController::ControlData &controlMode : controlModes) {
    if (controlMode.mode == CONTROL_MODE::MODE_PID && static_cast<CONTROL_SOURCE>(controlMode.source) == CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP) {
      return controlMode.pidCtrl->getSetpoint();
    }
    else if (controlMode.mode == CONTROL_MODE::MODE_OFF) {
      break;
    }
  }
  return 0;
}

void TempController::doFanUpdate()
{
  // update delta T
  if (supplyTemp.cfg.pin && returnTemp.cfg.pin)
    deltaT = returnTemp.val - supplyTemp.val;
  else if (deltaT != 0)
    deltaT = 0;

  // Loop control map, calculate pct, and update fan PWM signal
  uint8_t pout, pct;
  for (ControlData &value : controlModes) {
    switch (value.mode) {
      case CONTROL_MODE::MODE_TBL:
        for (const auto &fan : value.fans) {
          if (fan != nullptr) {
            pct = value.doTbl(fan->cfg.tbl.temp_pct_table);
            pout = static_cast<uint8_t>(round(map(pct, 0, 100, 0, 255)));
            fan->writePWM(pout);
          }
          else {
            break;
          }
        }
        break;
      case CONTROL_MODE::MODE_FIXED:
        pct = value.doFixed();
        pout = static_cast<uint8_t>(round(map(pct, 0, 100, 0, 255)));
        for (const auto &fan : value.fans) {
          if (fan != nullptr) {
            fan->writePWM(pout);
          }
          else {
            break;
          }
        }
        break;
      case CONTROL_MODE::MODE_PID:
        pct = value.doPID(caseTemp);
        pout = static_cast<uint8_t>(round(map(pct, 0, 100, 0, 255)));
        for (const auto &fan : value.fans) {
          if (fan != nullptr) {
            fan->writePWM(pout);
          }
          else {
            break;
          }
        }
        break;
      case CONTROL_MODE::MODE_OFF:
        break;
    }
  }
}

const float &TempController::getDeltaT() const
{
  return deltaT;
}

uint16_t TempController::getFanRPM(uint8_t i) const
{
  if (i < FAN_CNT)
    return fans[i]->rpm;
  else
    return 0;
}

const std::array<TempController::ControlData, FAN_CNT> &TempController::getControlModes() const
{
  return controlModes;
}
