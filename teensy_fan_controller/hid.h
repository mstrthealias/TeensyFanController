//
// Created by jd on 12/15/2019.
//

#ifndef TFC_HID_H
#define TFC_HID_H

#include <Arduino.h>
#include "temp_controller.h"
#include "hid_shared.h"

#ifdef USB_RAWHID_EN


class HID {
  private:
    const std::unique_ptr<TempController> &ctrl;
    RuntimeConfig &config;

    byte buffer[64];  // Teensy RawHID packets are 64 bytes

    HID_STATE state = HID_DATA;

    void setState(HID_STATE state);
    void logConfigChunk(uint8_t chunk);

    void setupPayloadData();
    void setupPayloadConfig(uint8_t chunk);

  public:
    HID(const std::unique_ptr<TempController> &ctrl, RuntimeConfig &config);

    uint8_t recv();
    uint8_t send();
};

#endif  //USB_RAWHID_EN

#endif //TFC_HID_H
