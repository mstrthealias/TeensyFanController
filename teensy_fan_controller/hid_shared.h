//
// Created by jd on 12/15/2019.
//

#ifndef TFC_HID_SHARED_H
#define TFC_HID_SHARED_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include  <cstdint>
typedef unsigned char byte;
#endif


// Payload types received by remote
#define HID_IN_PAYLOAD_REQ_CONFIG1       0xC0
#define HID_IN_PAYLOAD_REQ_CONFIG2       0xFF

// Payload types sent to remote
#define HID_OUT_PAYLOAD_DATA1         0xDA
#define HID_OUT_PAYLOAD_DATA2         0xDA

// Bi-directional payload types
#define HID_PAYLOAD_CONFIG1           0xC0
#define HID_PAYLOAD_CONFIG2           0xC1


#define FILL_ZEROS(buf, start, size)\
  memset((buf + start), 0x00, size - start)


enum HID_STATE : uint8_t {
  HID_DATA,
  HID_CONFIG,
  HID_DOWNLOAD,
};


#endif //TFC_HID_SHARED_H
