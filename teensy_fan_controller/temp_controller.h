//
// Created by jd on 11/26/2019.
//

#ifndef TFC_TEMP_CONTROLLER_H
#define TFC_TEMP_CONTROLLER_H

#include <utility>
#include <functional>
#include <map>
#include <PID_v1.h>
#include "runtime_config.h"
#include "core.h"


/**
   A single TempController instance is used by the main application to calculate fan PWM %.
*/
class TempController {
  public:
    struct ControlData;

    /**
       Each temp sensor may operate a PID controller through an instance of this controller.
    */
    class PIDController {
      private:
        const RuntimeConfig::PIDConfig &pidCfg;
        double setpoint;
        double in;
        double pct;
        PID pid;
        const uint16_t samplePeriod;

        unsigned long lastSincePercentsBelowLimit = 0;
        unsigned long lastSincePercentsAboveLimit = 0;
        bool lastPercentsAboveLimit = false;
        bool lastPercentsBelowLimit = false;

      public:
        PIDController(const RuntimeConfig::PIDConfig &pidCfg, uint16_t samplePeriod);

        uint8_t sample(const float &sample, const SensorData &caseTemp);

        const double& getSetpoint() const;
    };

    /**
       Used to group a set of fans to a unique control strategy.
    */
    struct ControlData {
      float *sample;
      PIDController *pidCtrl;
      std::array<const FanData*, 6> fans;
      uint16_t samplePeriod;
      uint8_t pct = 0;
      String label;

      ControlData();
      ControlData(float *sample, const FanData* firstFan, const uint16_t samplePeriod, const String &label);  // %-tbl constructor
      ControlData(SensorData *pidSensor, const FanData* firstFan, const uint16_t samplePeriod, const String &label);  // PID constructor
      ControlData(float *sample, const FanData* firstFan, const uint16_t samplePeriod, const uint8_t pct, const String &label);  // fixed-% constructor
      ControlData(ControlData&& that);  // move constructor
      ~ControlData();

      ControlData& operator=(ControlData&& that);  // move assignment operator

      // delete copy assignment
      ControlData(const ControlData& that) = delete;
      ControlData& operator=(const ControlData& that) = delete;

      uint8_t doPID(const SensorData &caseTemp);
      uint8_t doTbl(const auto &tempPctTbl);
      uint8_t doFixed();

      uint8_t getFanCount() const;
    };

    typedef std::pair<CONTROL_MODE, uint8_t> unique_ctrl_t;  // second part is CONTROL_SOURCE OR fixed fan %
    typedef std::map<unique_ctrl_t, ControlData> ctrl_map_t;

  private:
    const RuntimeConfig &config;

  public:
    SensorData &supplyTemp;  // Water Supply Temp
    SensorData &returnTemp;  // Water Return Temp
    SensorData &caseTemp;
    SensorData &aux1Temp;
    SensorData &aux2Temp;

    const std::array<const FanData*, 6> fans;

    TempController(const RuntimeConfig &config, uint16_t samplePeriod, SensorData &supplyTemp, SensorData &returnTemp, SensorData &caseTemp, SensorData &aux1Temp, SensorData &aux2Temp, const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6, const void (*setupHardware)(), const void (*saveConfig)());

    void configChanged(bool doSave = true);
    void doFanUpdate();

    float getDeltaT() const;
    const ctrl_map_t& getControlModes() const;

  private:
    const uint16_t samplePeriod;

    float deltaT;  // returnTemp - supplyTemp (0 if no returnTemp sensor)

    ctrl_map_t controlModes;  // std::map of unique control modes w/ related fans

    const void (*setupHardware)();  // callback to update pin muxing after config changes
    const void (*saveConfig)();  // callback to save config to EEPROM after config changes
};


#endif //TFC_TEMP_CONTROLLER_H
