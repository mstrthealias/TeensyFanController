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

std::unique_ptr<TempController> ctrl;
#ifdef USB_RAWHID_EN
std::unique_ptr<HID> hid;
#endif

// use 500ms timer interrupt to average pulses to RPM (and enable log logic)
IntervalTimer rpmTimer;

// use PERIOD ms timer interrupt to enable ADC read (and PID update)
IntervalTimer adcTimer;

RuntimeConfig config;  // saved in EEPROM (using __RuntimeConfig_v1)

SensorData supplyTemp = SensorData(config.tempSupply);  // Water Supply Temp
SensorData returnTemp = SensorData(config.tempReturn);  // Water Return Temp
SensorData caseTemp = SensorData(config.tempCase);  // Case Temp
SensorData auxTemp = SensorData(config.tempAux);
FanData fan1 = FanData(config.fan1);
FanData fan2 = FanData(config.fan2);
FanData fan3 = FanData(config.fan3);
FanData fan4 = FanData(config.fan4);
FanData fan5 = FanData(config.fan5);
FanData fan6 = FanData(config.fan6);

byte rbuffer[CONFIG_BYTES];


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

   Enables do_rpm() and do_log() logic.
*/
void timer_rpm_handler() {
  doRPM = true;
  doLog = true;
  doHIDSend = true;
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
  fan1.setupPin(interrupt_handler1);
  fan2.setupPin(interrupt_handler2);
  fan3.setupPin(interrupt_handler3);
  fan4.setupPin(interrupt_handler4);
  fan5.setupPin(interrupt_handler5);
  fan6.setupPin(interrupt_handler6);
}

void read_eeprom() {
  memset(rbuffer, '\0', CONFIG_BYTES);
  read_config(rbuffer, CONFIG_BYTES);
  config = RuntimeConfig::parse_bytes(rbuffer, CONFIG_BYTES);
  Serial.println("Config read from EEPROM");
}

void update_eeprom() {
  if (config.to_bytes(rbuffer, CONFIG_BYTES) == 0) {
    write_config(rbuffer, CONFIG_BYTES);
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

  setup_hardware();

  ctrl = std::make_unique<TempController>(config, PERIOD, supplyTemp, returnTemp, caseTemp, auxTemp, fan1, fan2, fan3, fan4, fan5, fan6, setup_hardware, update_eeprom);
#ifdef USB_RAWHID_EN
  hid = std::make_unique<HID>(ctrl, config);
#endif

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
  float pct;
  uint8_t pout;

  if (config.fan1.mode == MODE_PID || config.fan2.mode == MODE_PID || config.fan3.mode == MODE_PID || config.fan4.mode == MODE_PID || config.fan5.mode == MODE_PID || config.fan6.mode == MODE_PID) {
    // PID update
    pct = ctrl->sample(MODE_PID, supplyTemp.val);
    pout = (uint8_t) round(map(pct, 0, 100, 0, 255));
    fan1.writePWM(pout, MODE_PID);
    fan2.writePWM(pout, MODE_PID);
    fan3.writePWM(pout, MODE_PID);
    fan4.writePWM(pout, MODE_PID);
    fan5.writePWM(pout, MODE_PID);
    fan6.writePWM(pout, MODE_PID);
  }

  if (config.fan1.mode == MODE_PERCENT_TABLE || config.fan2.mode == MODE_PERCENT_TABLE || config.fan3.mode == MODE_PERCENT_TABLE || config.fan4.mode == MODE_PERCENT_TABLE || config.fan5.mode == MODE_PERCENT_TABLE || config.fan6.mode == MODE_PERCENT_TABLE) {
    // fan table update
    pct = ctrl->sample(MODE_PERCENT_TABLE, ctrl->getTableInputTemp());
    pout = (uint8_t) round(map(pct, 0, 100, 0, 255));
    fan1.writePWM(pout, MODE_PERCENT_TABLE);
    fan2.writePWM(pout, MODE_PERCENT_TABLE);
    fan3.writePWM(pout, MODE_PERCENT_TABLE);
    fan4.writePWM(pout, MODE_PERCENT_TABLE);
    fan5.writePWM(pout, MODE_PERCENT_TABLE);
    fan6.writePWM(pout, MODE_PERCENT_TABLE);
  }
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
    if (auxTemp.cfg.pin)
      auxTemp.samples[i] = analogRead(auxTemp.cfg.pin);
    delay(READ_DELAY);
  }

  // convert readings to tempC
  supplyTemp.doSample();
  returnTemp.doSample();
  caseTemp.doSample();
  auxTemp.doSample();
}

/*
   Macros for do_log().
*/

#define PRINT_RPM(str, f)\
  if (f.cfg.pinRPM) { Serial.print(str);Serial.print(f.rpm);Serial.print("; "); }
#define PRINT_TEMP(str, temp)\
  Serial.print(str); Serial.print(temp); Serial.print(" C; ");
#define PRINT_PCT(str, pct)\
  Serial.print(str); Serial.print(pct); Serial.print("%; ");

/**
   Writes data log to (usb raw hid) serial. Invoked once every 500ms.
*/
void do_log() {
  float pct = ctrl->getFanPercentPID();

  PRINT_TEMP("CWS-T ", supplyTemp.val);
  if (returnTemp.cfg.pin) {
    //PRINT_TEMP("CWR-T ", returnTemp.val);
    PRINT_TEMP("DeltaT ", ctrl->getDeltaT());
  }

  PRINT_TEMP("Set-Point ", ctrl->getTempSetpoint());

  if (caseTemp.cfg.pin) {
    PRINT_TEMP("Case-T ", caseTemp.val);
  }
  if (auxTemp.cfg.pin) {
    PRINT_TEMP("Aux-T ", auxTemp.val);
  }

  PRINT_PCT("PID-PWM ", pct);

  PRINT_RPM("RPM1 ", fan1);
  PRINT_RPM("RPM2 ", fan2);
  PRINT_RPM("RPM3 ", fan3);
  PRINT_RPM("RPM4 ", fan4);
  PRINT_RPM("RPM5 ", fan5);
  PRINT_RPM("RPM6 ", fan6);

  Serial.println();
}


#ifdef USB_RAWHID_EN
/**
   Reads command from HID raw.
*/
void do_hid_recv() {
  hid->recv();  // TODO do something and return non void
}

/**
   Transmits HID raw data packet(s) (which packet sent depends on HID state). Invoked once every 500ms.
*/
void do_hid_send() {
  if (hid->send() == 0) {
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
