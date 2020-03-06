//
// Created by jd on 11/26/2019.
//

#ifndef TFC_TEMP_CONTROLLER_H
#define TFC_TEMP_CONTROLLER_H

#include <array>
#include <PID_v1.h>
#include "runtime_config.h"
#include "core.h"


/**
   A single TempController instance is used by the main application to calculate fan PWM %.
*/
class TempController {
  public:
    typedef PIDt<float, float, float> PID;

    struct ControlData;

    /**
       Each temp sensor may operate a PID controller through an instance of this controller.
    */
    class PIDController {
      private:
        const RuntimeConfig::PIDConfig &pidCfg;
        float setpoint;
        float in;
        float pct;
        PID pid;

        unsigned long lastSincePercentsBelowLimit = 0;
        unsigned long lastSincePercentsAboveLimit = 0;
        bool lastPercentsAboveLimit = false;
        bool lastPercentsBelowLimit = false;

      public:
        PIDController(const RuntimeConfig::PIDConfig &pidCfg);

        uint8_t sample(const float &sample, const SensorData &caseTemp);

        const float &getSetpoint() const;
    };

    /**
       Used to group a set of fans to a unique control strategy.
    */
    struct ControlData {
      float *sample;
      PIDController *pidCtrl;
      std::array<FanData *, FAN_CNT> fans;
      CONTROL_MODE mode;
      uint8_t source;  //CONTROL_SOURCE (or fixed fan %)
      uint8_t pct = 0;
      uint8_t fanNo = 0;  // the first fan associated w/ this control data
      String label;

      ControlData();
      ~ControlData();

      void resetPIDCtrl();
      void setPctTable(float *const sample, const CONTROL_MODE mode, const CONTROL_SOURCE source, const uint8_t fanNo, const String &label);  // %-tbl constructor
      void setPID(SensorData *const pidSensor, const CONTROL_MODE mode, const CONTROL_SOURCE source, const uint8_t fanNo, const String &label);  // PID constructor
      void setFixed(float *const sample, const CONTROL_MODE mode, const uint8_t source, const uint8_t pct, const uint8_t fanNo, const String &label);  // fixed-% constructor
      void reset();

      // delete copy assignment
      ControlData(const ControlData &that) = delete;
      ControlData &operator=(const ControlData &that) = delete;

      uint8_t doPID(const SensorData &caseTemp);
      uint8_t doTbl(const auto &tempPctTbl);
      uint8_t doFixed();

      uint8_t getFanCount() const;
    };


  public:
    static const uint16_t SAMPLE_PERIOD = PERIOD_UPDATE;

    RuntimeConfig &config;

    SensorData &supplyTemp;  // Water Supply Temp
    SensorData &returnTemp;  // Water Return Temp
    SensorData &caseTemp;
    SensorData &aux1Temp;
    SensorData &aux2Temp;

    TempController(RuntimeConfig &config,
                   SensorData &supplyTemp, SensorData &returnTemp, SensorData &caseTemp, SensorData &aux1Temp, SensorData &aux2Temp,
                   FanData &fan1, FanData &fan2, FanData &fan3, FanData &fan4, FanData &fan5, FanData &fan6,
                   void (*const setupHardware)(),
                   void (*const saveConfig)());

    void configChanged(bool doSave = true);
    void doFanUpdate();
    void resetControlModes();

    const float &getDeltaT() const;
    float getPIDSupplyTempSetpoint() const;
    float getPIDAux1TempSetpoint() const;
    const FanData &getFan(uint8_t i) const;
    const std::array<ControlData, FAN_CNT> &getControlModes() const;


  private:
    const std::array<FanData *const, FAN_CNT> fans;
    std::array<ControlData, FAN_CNT> controlModes;

    float deltaT;  // returnTemp - supplyTemp (0 if no returnTemp sensor)

    ControlData &findOrCreateControlMode(CONTROL_MODE mode, uint8_t source, uint8_t i);

    void (*const setupHardware)();  // callback to update pin muxing after config changes
    void (*const saveConfig)();  // callback to save config to EEPROM after config changes
};


#endif //TFC_TEMP_CONTROLLER_H
