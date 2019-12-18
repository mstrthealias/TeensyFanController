//
// Created by jd on 11/26/2019.
//

#ifndef TFC_TEMP_CONTROLLER_H
#define TFC_TEMP_CONTROLLER_H

#include <PID_v1.h>
#include "runtime_config.h"
#include "core.h"


class TempController {
  private:
    const RuntimeConfig &config;

    // PID values
    double _pid_input;
    double _pid_pct;
    double _pid_setpoint;

    PID pid;

  public:
    const SensorData &supplyTemp;  // Water Supply Temp
    const SensorData &returnTemp;  // Water Return Temp
    const SensorData &caseTemp;  // Case Temp
    const SensorData &auxTemp;

    const FanData &fan1, &fan2, &fan3, &fan4, &fan5, &fan6;

    TempController(const RuntimeConfig &config, uint16_t samplePeriod, const SensorData &supplyTemp, const SensorData &returnTemp, const SensorData &caseTemp, const SensorData &auxTemp, const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6, const void (*setupHardware)(), const void (*saveConfig)());

    void configChanged();

    float getTempSetpoint();
    float getFanPercentPID();
    float getFanPercentTbl();
    float getDeltaT();
    float getTableInputTemp();
    float sample(CONTROL_MODE mode, float sample);

  private:
    // percent table values
    float _tbl_pct;

    // PID controls
    unsigned long lastSincePercentsBelowLimit = 0;
    unsigned long lastSincePercentsAboveLimit = 0;

    bool lastPercentsAboveLimit = false;
    bool lastPercentsBelowLimit = false;

    const void (*setupHardware)();  // callback to update pin muxing after config changes
    const void (*saveConfig)();  // callback to save config to EEPROM after config changes
};


#endif //TFC_TEMP_CONTROLLER_H
