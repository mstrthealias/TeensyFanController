#include <memory>

#include "moving_average.h"
#include "runtime_config.h"
#include "temp_controller.h"
#include "core.h"

#ifdef USB_RAWHID_EN
#include "hid.h"
#endif

// timers invoke ISR callbacks that toggle these flags, enabling the logic in loop(void):
volatile bool doRPM = false;
volatile bool doADC = false;
volatile bool doPID = false;
volatile bool doLog = false;
volatile bool doHIDSend = false;

// use 500ms timer interrupt to average pulses to RPM (and enable log logic)
IntervalTimer rpmTimer;

// use PERIOD ms timer interrupt to enable ADC read (and PID update)
IntervalTimer adcTimer;

RuntimeConfig config;  // saved in EEPROM (using __RuntimeConfig_v1)

SensorData supplyTemp = SensorData(config.tempSupply, "CWS-T");  // Water Supply Temp
SensorData returnTemp = SensorData(config.tempReturn, "CWR-T");  // Water Return Temp
SensorData caseTemp = SensorData(config.tempCase, "Case-T");
SensorData aux1Temp = SensorData(config.tempAux1, "Aux1-T");
SensorData aux2Temp = SensorData(config.tempAux2, "Aux2-T");
FanData fan1 = FanData(config.fan1, "1");
FanData fan2 = FanData(config.fan2, "2");
FanData fan3 = FanData(config.fan3, "3");
FanData fan4 = FanData(config.fan4, "4");
FanData fan5 = FanData(config.fan5, "5");
FanData fan6 = FanData(config.fan6, "6");

byte rbuffer[CONFIG_BYTES];  // EEPROM buffer
uint8_t logIntervalCnt = 0;

// needed for controller function pointers:
void setup_hardware();
void update_eeprom();

// primary ctrl/TempController
TempController ctrl(config, PERIOD, supplyTemp, returnTemp, caseTemp, aux1Temp, aux2Temp, fan1, fan2, fan3, fan4, fan5, fan6, setup_hardware, update_eeprom);

// HID controller
#ifdef USB_RAWHID_EN
HID hid(ctrl);
#endif


/**
   ISR called from PERIOD ms (100ms) timer (adcTimer).

   Enables do_adc() and do_pid() logic.
*/
void timer_adc_handler() {
  doADC = true;
  doPID = true;
}

/**
   ISR called from PERIOD ms (500ms) timer (rpmTimer).

   Enables do_rpm(), and do_log() (every 10 calls).
*/
void timer_rpm_handler() {
  doRPM = true;
  doHIDSend = true;
  if (++logIntervalCnt >= 10) {
    logIntervalCnt = 0;
    doLog = true;
  }
}

/*
   Interrupt handlers, bound for each fan RPM signal.
*/

void interrupt_handler1() {
  fan1.pulse_counter++;
}
void interrupt_handler2() {
  fan2.pulse_counter++;
}
void interrupt_handler3() {
  fan3.pulse_counter++;
}
void interrupt_handler4() {
  fan4.pulse_counter++;
}
void interrupt_handler5() {
  fan5.pulse_counter++;
}
void interrupt_handler6() {
  fan6.pulse_counter++;
}

void setup_hardware() {
  supplyTemp.setupPin();
  returnTemp.setupPin();
  caseTemp.setupPin();
  aux1Temp.setupPin();
  aux2Temp.setupPin();
  fan1.setupPin(interrupt_handler1);
  fan2.setupPin(interrupt_handler2);
  fan3.setupPin(interrupt_handler3);
  fan4.setupPin(interrupt_handler4);
  fan5.setupPin(interrupt_handler5);
  fan6.setupPin(interrupt_handler6);
}

void read_eeprom() {
  memset(rbuffer, '\0', CONFIG_BYTES);
#ifndef DISABLE_EEPROM
  read_config(rbuffer, CONFIG_BYTES);
#endif
  config = RuntimeConfig::parse_bytes(rbuffer, CONFIG_BYTES);
  Serial.println("Config read from EEPROM");
}

void update_eeprom() {
  if (config.to_bytes(rbuffer, CONFIG_BYTES) == 0) {
#ifndef DISABLE_EEPROM
    write_config(rbuffer, CONFIG_BYTES);
#endif
    Serial.println("Config written to EEPROM");
  }
}


void setup(void) {
  Serial.begin(9600);

#ifndef TEENSY_4
  analogReference(EXTERNAL);
#endif
  analogReadResolution(ADC_RESOLUTION);

  read_eeprom();

  // initialize controller (calls setup_hardware())
  ctrl.configChanged(false);

  rpmTimer.begin(timer_rpm_handler, 500000);
  adcTimer.begin(timer_adc_handler, PERIOD * 1000);
}

/**
   Flush pulse_counter to RPM (2 pulse per revolution, over 0.5s...) for each fan. Invoked once every 500ms.
*/
void do_rpm() {
  fan1.doRPM();
  fan2.doRPM();
  fan3.doRPM();
  fan4.doRPM();
  fan5.doRPM();
  fan6.doRPM();
}

/**
   Calculate fan PWN % (PID & table), and write PWM outputs. Invoked once every 100ms.
*/
void do_pid() {
  ctrl.doFanUpdate();
}

/**
   Read ADC signals. Invoked once every 500ms.
*/
void do_adc() {
  // average NUMSAMPLES of samples, each delayed by READ_DELAY
  uint8_t i;
  for (i = 0; i < NUMSAMPLES; i++) {
    supplyTemp.samples[i] = analogRead(supplyTemp.cfg.pin);
    if (returnTemp.cfg.pin)
      returnTemp.samples[i] = analogRead(returnTemp.cfg.pin);
    if (caseTemp.cfg.pin)
      caseTemp.samples[i] = analogRead(caseTemp.cfg.pin);
    if (aux1Temp.cfg.pin)
      aux1Temp.samples[i] = analogRead(aux1Temp.cfg.pin);
    if (aux2Temp.cfg.pin)
      aux2Temp.samples[i] = analogRead(aux2Temp.cfg.pin);
    delay(READ_DELAY);
  }

  // convert readings to tempC
  supplyTemp.doSample();
  returnTemp.doSample();
  caseTemp.doSample();
  aux1Temp.doSample();
  aux2Temp.doSample();
}

/*
   Macros for do_log().
*/

#define PRINT_RPM(str, f)\
  if (f->cfg.pinRPM) { Serial.print("; ");Serial.print(str);Serial.print(f->rpm); }
#define PRINT_TEMP(str, temp)\
  Serial.print(str); Serial.print(temp); Serial.print(" C; ");
#define PRINT_PCT(str, pct)\
  Serial.print(str); Serial.print(pct); Serial.print("%");

/**
   Writes data log to (usb raw hid) serial. Invoked once every 500ms.
*/
void do_log() {
  PRINT_TEMP("CWS-T ", supplyTemp.val);
  if (returnTemp.cfg.pin) {
    //PRINT_TEMP("CWR-T ", returnTemp.val);
    PRINT_TEMP("DeltaT ", ctrl.getDeltaT());
  }

  if (caseTemp.cfg.pin) {
    PRINT_TEMP("Case-T ", caseTemp.val);
  }
  if (aux1Temp.cfg.pin) {
    PRINT_TEMP("Aux1-T ", aux1Temp.val);
  }
  if (aux2Temp.cfg.pin) {
    PRINT_TEMP("Aux2-T ", aux2Temp.val);
  }

  // Iterate over the map
  uint8_t i = 0;
  for (const auto &value : ctrl.getControlModes()) {
    if (value.mode == CONTROL_MODE::MODE_OFF)
      break;  // remaining control modes are off

    if (i++ != 0)
      Serial.print("; ");

    Serial.print(" (" + value.label + ": ");

    if (value.mode == CONTROL_MODE::MODE_PID) {
      PRINT_TEMP("Setpoint ", value.pidCtrl->getSetpoint());
      PRINT_PCT("PWM ", value.pct);
    }
    // do not print % for tbl, as there may be multiple tables in this config
//    else if (value.mode == CONTROL_MODE::MODE_TBL) {
//      PRINT_PCT("PWM ", value.pct);
//    }
    else if (value.mode == CONTROL_MODE::MODE_FIXED) {
      PRINT_PCT("PWM ", value.pct);
    }

    // loop and print RPM for each fan associated to this control mode
    for (const auto &fan : value.fans) {
      if (fan != nullptr) {
        PRINT_RPM("RPM" + fan->lbl + " ", fan);
      }
      else {
        break;
      }
    }
    Serial.print(")");
  }
  Serial.println();
}


#ifdef USB_RAWHID_EN
/**
   Reads command from HID raw.
*/
void do_hid_recv() {
  hid.recv();
}

/**
   Transmits HID raw data packet(s) (which packet sent depends on HID state). Invoked once every 500ms.
*/
void do_hid_send() {
  if (hid.send() < 0) {
    Serial.println(F("Unable to transmit packet"));
  }
}
#endif


void loop(void) {
  if (doADC) {
    doADC = false;
    do_adc();
  }

  if (doPID) {
    doPID = false;
    do_pid();
  }

#ifdef USB_RAWHID_EN
  do_hid_recv();

  if (doHIDSend) {
    doHIDSend = false;
    do_hid_send();
  }
#endif

  if (doRPM) {
    doRPM = false;
    do_rpm();
  }

  if (doLog) {
    doLog = false;
    do_log();
  }

  //delay(1);
}
