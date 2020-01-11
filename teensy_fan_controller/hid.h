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
    TempController &ctrl;

    byte buffer[64];  // Teensy RawHID packets are 64 bytes
    byte config_bytes[CONFIG_BYTES];

    HID_STATE state = HID_DATA;

    void setState(HID_STATE state);
    void logConfigChunk(uint8_t chunk);

    void setupPayloadData();
    void setupPayloadConfig(uint8_t chunk);

  public:
    HID(TempController &ctrl);

    uint8_t recv();
    uint8_t send();
};

#endif  //USB_RAWHID_EN

#endif //TFC_HID_H
