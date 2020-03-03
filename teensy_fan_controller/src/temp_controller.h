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
        PID<float, float, float> pid;
        const uint16_t samplePeriod;

        unsigned long lastSincePercentsBelowLimit = 0;
        unsigned long lastSincePercentsAboveLimit = 0;
        bool lastPercentsAboveLimit = false;
        bool lastPercentsBelowLimit = false;

      public:
        PIDController(const RuntimeConfig::PIDConfig &pidCfg, uint16_t samplePeriod);

        uint8_t sample(const float &sample, const SensorData &caseTemp);

        const float &getSetpoint() const;
    };

    /**
       Used to group a set of fans to a unique control strategy.
    */
    struct ControlData {
      float *sample;
      PIDController *pidCtrl;
      std::array<const FanData *, FAN_CNT> fans;
      CONTROL_MODE mode;
      uint8_t source;  //CONTROL_SOURCE (or fixed fan %)
      uint16_t samplePeriod;
      uint8_t pct = 0;
      String label;

      ControlData();
      ~ControlData();

      void resetPIDCtrl();
      void setPctTable(float *sample, CONTROL_MODE mode, CONTROL_SOURCE source, const uint16_t samplePeriod, const String &label);  // %-tbl constructor
      void setPID(SensorData *pidSensor, CONTROL_MODE mode, CONTROL_SOURCE source, const uint16_t samplePeriod, const String &label);  // PID constructor
      void setFixed(float *sample, CONTROL_MODE mode, uint8_t source, const uint16_t samplePeriod, const uint8_t pct, const String &label);  // fixed-% constructor
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
    RuntimeConfig &config;

    SensorData &supplyTemp;  // Water Supply Temp
    SensorData &returnTemp;  // Water Return Temp
    SensorData &caseTemp;
    SensorData &aux1Temp;
    SensorData &aux2Temp;

    TempController(RuntimeConfig &config, uint16_t samplePeriod,
                   SensorData &supplyTemp, SensorData &returnTemp, SensorData &caseTemp, SensorData &aux1Temp, SensorData &aux2Temp,
                   const FanData &fan1, const FanData &fan2, const FanData &fan3, const FanData &fan4, const FanData &fan5, const FanData &fan6,
                   void (*setupHardware)(),
                   void (*saveConfig)());

    void configChanged(bool doSave = true);
    void doFanUpdate();
    void resetControlModes();

    float getDeltaT() const;
    float getPIDSupplyTempSetpoint() const;
    uint16_t getFanRPM(uint8_t i) const;
    const std::array<ControlData, FAN_CNT> &getControlModes() const;


  private:
    const std::array<const FanData *, FAN_CNT> fans;
    std::array<ControlData, FAN_CNT> controlModes;

    float deltaT;  // returnTemp - supplyTemp (0 if no returnTemp sensor)

    const uint16_t samplePeriod;

    ControlData &findOrCreateControlMode(CONTROL_MODE mode, uint8_t source, uint8_t i);

    void (*setupHardware)();  // callback to update pin muxing after config changes
    void (*saveConfig)();  // callback to save config to EEPROM after config changes
};


#endif //TFC_TEMP_CONTROLLER_H
