//
// Created by jd on 12/15/2019.
//

#ifndef TFC_HID_H
#define TFC_HID_H

#include <Arduino.h>
#include <memory>
#include "temp_controller.h"
#include "core.h"


#ifdef USB_RAWHID_EN

// Payload types received by remote
#define HID_IN_PAYLOAD_REQ_CONFIG1       0xC0
#define HID_IN_PAYLOAD_REQ_CONFIG2       0xFF

// Payload types sent to remote
#define HID_OUT_PAYLOAD_DATA1         0xDA
#define HID_OUT_PAYLOAD_DATA2         0xDA

// Bi-directional payload types
#define HID_PAYLOAD_CONFIG1           0xC0
#define HID_PAYLOAD_CONFIG2           0xC1
#define HID_PAYLOAD_CONFIG_B1         0xC0
#define HID_PAYLOAD_CONFIG_B2         0xC2
#define HID_PAYLOAD_CONFIG_C1         0xC0
#define HID_PAYLOAD_CONFIG_C2         0xC3


#define FILL_ZEROS(buf, start, size)\
  memset((buf + start), 0x00, size - start)


enum HID_STATE : uint8_t {
  HID_DATA,
  HID_CONFIG,
  HID_DOWNLOAD,
};


class HID {
  private:
    const std::unique_ptr<TempController> &ctrl;
    RuntimeConfig &config;

    byte buffer[64];  // Teensy RawHID packets are 64 bytes

    HID_STATE state = HID_DATA;

    void setState(HID_STATE state);
    void logConfigChunk(uint8_t chunk);

    void setupPayloadData();
    void setupPayloadConfig();
    void setupPayloadConfigB();
    void setupPayloadConfigC();

  public:
    HID(const std::unique_ptr<TempController> &ctrl, RuntimeConfig &config);

    uint8_t recv();
    uint8_t send();
};

#endif  //USB_RAWHID_EN

#endif //TFC_HID_H
