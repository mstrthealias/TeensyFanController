//
// Created by jd on 11/26/2019.
//

#include <array>

#include "temp_controller.h"


TempController::PIDController::PIDController(const RuntimeConfig::PIDConfig &pidCfg, const uint16_t samplePeriod)
  : pidCfg(pidCfg), setpoint(pidCfg.setpoint), in(0), pct(0),
    pid(&in, &pct, &setpoint, pidCfg.gain_p, pidCfg.gain_i, pidCfg.gain_d, REVERSE),
    samplePeriod(samplePeriod)
{
  // configure PID
  pid.SetSampleTime(samplePeriod);
  if (setpoint >= pidCfg.setpoint_max)
    pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max2);
  else
    pid.SetOutputLimits(pidCfg.pwm_percent_min, pidCfg.pwm_percent_max1);
  pid.SetMode(AUTOMATIC);  // start PID
}

uint8_t TempController::PIDController::sample(const float &sample, const SensorData &caseTemp)
{
  in = static_cast<double>(sample);
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

const double& TempController::PIDController::getSetpoint() const
{
  return setpoint;
}


TempController::ControlData::ControlData() : sample(nullptr), pidCtrl(nullptr), fans{{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}}, samplePeriod(0), label("")
{}

TempController::ControlData::ControlData(float *sample, const FanData* firstFan, const uint16_t samplePeriod, const String &label)
  : sample(sample),
    pidCtrl(nullptr),
    fans({{firstFan, nullptr, nullptr, nullptr, nullptr, nullptr}}),
    samplePeriod(samplePeriod), label(label)
{}

TempController::ControlData::ControlData(SensorData *pidSensor, const FanData* firstFan, const uint16_t samplePeriod, const String &label)
  : sample(&pidSensor->val),
    pidCtrl(new PIDController(pidSensor->cfg.pid, samplePeriod)),
    fans({{firstFan, nullptr, nullptr, nullptr, nullptr, nullptr}}),
    samplePeriod(samplePeriod), label(label)
{}

TempController::ControlData::ControlData(float *sample, const FanData* firstFan, const uint16_t samplePeriod, const uint8_t pct, const String &label)
  : sample(sample),
    pidCtrl(nullptr),
    fans({{firstFan, nullptr, nullptr, nullptr, nullptr, nullptr}}),
    samplePeriod(samplePeriod), pct(pct), label(label)
{}

// move constructor
TempController::ControlData::ControlData(ControlData&& that)
  : sample(that.sample),
    pidCtrl(that.pidCtrl),
    fans(that.fans),
    samplePeriod(that.samplePeriod), pct(that.pct), label(that.label)
{
  that.pidCtrl = nullptr;
}

TempController::ControlData::~ControlData()
{
  if (pidCtrl != nullptr) {
    delete pidCtrl;
    pidCtrl = nullptr;
  }
}

// move assignment operator
TempController::ControlData& TempController::ControlData::operator=(ControlData&& that)
{
  using std::swap;
  swap(pidCtrl, that.pidCtrl);

  sample = that.sample;
  pidCtrl = that.pidCtrl;
  fans = that.fans;
  samplePeriod = that.samplePeriod;
  pct = that.pct;
  label = that.label;

  return *this;
}

uint8_t TempController::ControlData::doPID(const SensorData &caseTemp)
{
  // check pidCtrl != nullptr ?
  return pct = static_cast<uint8_t>(round(pidCtrl->sample(*sample, caseTemp)));
}

uint8_t TempController::ControlData::doTbl(const auto &tempPctTbl)
{
  // Calculates Fan % using only tempPercentTable
  const float &reading = *sample;
  uint8_t lastTemp = 0, lastPct = 0,
          curTemp, curPct, i;
  for (i = 0; i < 9; i++) {
    curTemp = tempPctTbl[i][0];
    curPct = tempPctTbl[i][1];
    if (reading <= curTemp) {  // use this entry
      if (curPct >= 100) {
        return pct = 100;
      }
      else if (i > 0) {  // calc % between lastEntry and this entry
        return pct = (1 - ((curTemp - reading) / (curTemp - lastTemp))) * (curPct - lastPct) + lastPct;
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
  for (i = 0; i < fans.size(); i++) {
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

TempController::TempController(const RuntimeConfig &config, uint16_t samplePeriod, SensorData &supplyTemp, SensorData &returnTemp, SensorData &caseTemp, SensorData &aux1Temp, SensorData &aux2Temp, const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6, const void (*setupHardware)(), const void (*saveConfig)()) :
  config(config),
  supplyTemp(supplyTemp), returnTemp(returnTemp), caseTemp(caseTemp), aux1Temp(aux1Temp), aux2Temp(aux2Temp),
  fans{&fan1, &fan2, &fan3, &fan4, &fan5, &fan6},
  samplePeriod(samplePeriod), deltaT{0}, setupHardware(setupHardware), saveConfig(saveConfig)
{
  // turn the PID on
  configChanged(false);
}

void TempController::configChanged(bool doSave)
{
  if (doSave)
    saveConfig();  // save config into EEPROM

  // clear/rebuild control schemes (unique by control mode + sensor source)
  controlModes.clear();  // clear existing control schemes
  unique_ctrl_t key;
  int i, j;
  for (i = 0; i < 6; i++) {
    key = std::make_pair(fans[i]->cfg.mode, fans[i]->cfg.mode == CONTROL_MODE::MODE_FIXED ? static_cast<uint8_t>(fans[i]->cfg.ratio*100) : static_cast<uint8_t>(fans[i]->cfg.source));
    const auto &search = controlModes.find(key);
    if (search != controlModes.end()) {
      // add this fan to fans array
      auto &trackedFans = search->second.fans;
      for (j = 0; j < 6; j++) {
        if (trackedFans[j] == nullptr) {
          trackedFans[j] = fans[i];
          break;
        }
      }
    }
    else {
      if (fans[i]->cfg.mode == CONTROL_MODE::MODE_PID) {
        switch (static_cast<CONTROL_SOURCE>(key.second)) {
          case CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&supplyTemp, fans[i], samplePeriod, MK_CONTROL_LABEL(supplyTemp.lbl, "PID")}));
            break;
          case CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&returnTemp, fans[i], samplePeriod, MK_CONTROL_LABEL(returnTemp.lbl, "PID")}));
            break;
          case CONTROL_SOURCE::SENSOR_CASE_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&caseTemp, fans[i], samplePeriod, MK_CONTROL_LABEL(caseTemp.lbl, "PID")}));
            break;
          case CONTROL_SOURCE::SENSOR_AUX1_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&aux1Temp, fans[i], samplePeriod, MK_CONTROL_LABEL(aux1Temp.lbl, "PID")}));
            break;
          case CONTROL_SOURCE::SENSOR_AUX2_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&aux2Temp, fans[i], samplePeriod, MK_CONTROL_LABEL(aux2Temp.lbl, "PID")}));
            break;
          case CONTROL_SOURCE::VIRTUAL_DELTA_TEMP:  // PID on virtual temp is not supported
            break;
        }
      }
      else if (fans[i]->cfg.mode == CONTROL_MODE::MODE_TBL) {
        switch (static_cast<CONTROL_SOURCE>(key.second)) {
          case CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&supplyTemp.val, fans[i], samplePeriod, MK_CONTROL_LABEL(supplyTemp.lbl, "%-table")}));
            break;
          case CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&returnTemp.val, fans[i], samplePeriod, MK_CONTROL_LABEL(returnTemp.lbl, "%-table")}));
            break;
          case CONTROL_SOURCE::SENSOR_CASE_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&caseTemp.val, fans[i], samplePeriod, MK_CONTROL_LABEL(caseTemp.lbl, "%-table")}));
            break;
          case CONTROL_SOURCE::SENSOR_AUX1_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&aux1Temp.val, fans[i], samplePeriod, MK_CONTROL_LABEL(aux1Temp.lbl, "%-table")}));
            break;
          case CONTROL_SOURCE::SENSOR_AUX2_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&aux2Temp.val, fans[i], samplePeriod, MK_CONTROL_LABEL(aux2Temp.lbl, "%-table")}));
            break;
          case CONTROL_SOURCE::VIRTUAL_DELTA_TEMP:
            controlModes.insert(std::make_pair(key, ControlData{&deltaT, fans[i], samplePeriod, MK_CONTROL_LABEL("DeltaT", "%-table")}));
            break;
        }
      }
      else if (fans[i]->cfg.mode == CONTROL_MODE::MODE_FIXED) {
        // Note: fixed % fans pass deltaT to sample, although it is unused
        controlModes.insert(std::make_pair(key, ControlData{&deltaT, fans[i], samplePeriod, key.second, "Fixed"}));
      }
    }
  }

  setupHardware();  // setup pin muxing
}

void TempController::doFanUpdate()
{
  deltaT = getDeltaT();  // update delta T

  // Loop control map, calculate pct, and update fan PWM signal
  uint8_t pout, pct;
  ctrl_map_t::iterator iter;
  for (iter = controlModes.begin(); iter != controlModes.end(); iter++) {
    const unique_ctrl_t &key = iter->first;
    ControlData &value = iter->second;
    switch (key.first) {
      case CONTROL_MODE::MODE_TBL:
        for (const auto &fan : value.fans) {
          if (fan != nullptr) {
            pct = value.doTbl(fan->cfg.tbl.temp_pct_table);
            pout = static_cast<uint8_t>(round(map(pct, 0, 100, 0, 255)));
            fan->writePWM(pout, CONTROL_MODE::MODE_TBL);
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
            fan->writePWM(pout, CONTROL_MODE::MODE_FIXED);
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
            fan->writePWM(pout, CONTROL_MODE::MODE_PID);
          }
          else {
            break;
          }
        }
        break;
    }
  }
}

float TempController::getDeltaT() const
{
  if (returnTemp.cfg.pin)
    return returnTemp.val - supplyTemp.val;
  else
    return 0;
}

const TempController::ctrl_map_t& TempController::getControlModes() const
{
  return controlModes;
}
