//
// Created by jd on 12/15/2019.
//

#include <memory>
#include "hid.h"
#include "core.h"


#ifdef USB_RAWHID_EN


HID::HID(TempController &ctrl) : ctrl(ctrl)
{
}

uint8_t HID::send()
{
  uint16_t n = 0;
  if (state == HID_DATA) {
    // transmit data packet
    setupPayloadData();
    n = RawHID.send(buffer, 100);
  }
  else if (state == HID_CONFIG) {
    // config requested, transmit (3) config packets

    // copy entire configuration into config_bytes
    if (ctrl.config.to_bytes(config_bytes, CONFIG_BYTES) != 0) {
      return -1;  // TODO errno
    }

    setupPayloadConfig(0);
    n = RawHID.send(buffer, 100);  // TODO handle failures
    setupPayloadConfig(1);
    n += RawHID.send(buffer, 100);
    setupPayloadConfig(2);
    n += RawHID.send(buffer, 100);
    setupPayloadConfig(3);
    n += RawHID.send(buffer, 100);
    setupPayloadConfig(4);
    n += RawHID.send(buffer, 100);
    setupPayloadConfig(5);
    n += RawHID.send(buffer, 100);

    // setup last payload
    buffer[0] = HID_PAYLOAD_CONFIG1;
    buffer[1] = HID_PAYLOAD_CONFIG2 + 6;
    memcpy((buffer + 2), (config_bytes + 6 * CHUNK_SIZE), CONFIG_BYTES - 6 * CHUNK_SIZE);  // place chunk (48 bytes) in buffer
    FILL_ZEROS(buffer, (CONFIG_BYTES - 6 * CHUNK_SIZE + 2), CONFIG_BYTES);
    buffer[63] = HID_DATA;  // put next state at the end
    n += RawHID.send(buffer, 100);

    setState(HID_DATA);

    if (n < sizeof(buffer) * 7) {
      Serial.println("HID SEND ERROR");
      return -1;
    }
  }
  return n;
}

uint8_t HID::recv()
{
  uint8_t n = 0;
  n = RawHID.recv(buffer, 0);  // 0 timeout = do not wait
  if (n > 0) {
    if (buffer[0] == HID_IN_PAYLOAD_REQ_CONFIG1 && buffer[1] == HID_IN_PAYLOAD_REQ_CONFIG2 && state != HID_DOWNLOAD) {
      // Remote requested configuration payload
      setState(HID_CONFIG);  // transmit config in send()
    }
    else if (buffer[0] == HID_PAYLOAD_CONFIG1 && buffer[1] == HID_PAYLOAD_CONFIG2 && state != HID_DOWNLOAD) {
      // Remote is sending configuration payload chunk 1
      setState(HID_DOWNLOAD);  // expecting sequential config payloads
      memset(config_bytes, '\0', CONFIG_BYTES);
      memcpy(config_bytes, (buffer + 2), CHUNK_SIZE);
      logConfigChunk(1);
    }
    else if (buffer[0] == HID_PAYLOAD_CONFIG1 && buffer[1] > HID_PAYLOAD_CONFIG2 && buffer[1] <= HID_PAYLOAD_CONFIG2 + 6 && state == HID_DOWNLOAD) {
      // Remote is sending configuration payload chunk n
      if (buffer[1] == HID_PAYLOAD_CONFIG2 + 6)
        memcpy((config_bytes + 6 * CHUNK_SIZE), (buffer + 2), CONFIG_BYTES - 6 * CHUNK_SIZE);
      else
        memcpy((config_bytes + (buffer[1] - HID_PAYLOAD_CONFIG2) * CHUNK_SIZE), (buffer + 2), CHUNK_SIZE);

      logConfigChunk(buffer[1] - HID_PAYLOAD_CONFIG2 + 1);

      if (buffer[1] == HID_PAYLOAD_CONFIG2 + 6) {
        // last packet, switch to DAT mode
        setState(HID_DATA);

        // update config from received bytes
        ctrl.config = RuntimeConfig::parse_bytes(config_bytes, CONFIG_BYTES);
        ctrl.configChanged();  // trigger hardware/logic updates
      }
    }
    else if (state == HID_DOWNLOAD) {
      Serial.println("Unexpected packet received while in HID_DOWNLOAD state");
      memset(config_bytes, '\0', CONFIG_BYTES);
      setState(HID_DATA);
    }
  }
  return n;
}

void HID::setState(HID_STATE state)
{
  this->state = state;
  Serial.print("HID state=");
  Serial.println((
                   state == HID_CONFIG ? "HID_CONFIG"
                   : state == HID_DOWNLOAD ? "HID_DOWNLOAD"
                   : state == HID_DATA ? "HID_DATA"
                   : "?"
                 ));
}

void HID::logConfigChunk(uint8_t chunk)
{
  Serial.print("Config chunk ");
  Serial.print(chunk);
  Serial.println(" downloaded");
}

void HID::setupPayloadConfig(uint8_t chunk)
{
  // first 2 bytes are a signature
  buffer[0] = HID_PAYLOAD_CONFIG1;
  buffer[1] = HID_PAYLOAD_CONFIG2 + chunk;

  // place first chunk (48 bytes) in HID buffer
  memcpy((buffer + 2), (config_bytes + chunk * CHUNK_SIZE), CHUNK_SIZE);

  FILL_ZEROS(buffer, (CHUNK_SIZE + 2), sizeof(buffer));
  buffer[63] = HID_CONFIG;  // put next state at the end
}

void HID::setupPayloadData()
{
  uint64_t val;
  uint16_t rpm;

  // first 2 bytes are a signature
  buffer[0] = HID_OUT_PAYLOAD_DATA1;
  buffer[1] = HID_OUT_PAYLOAD_DATA2;

  // write values into buffer
  memcpy((buffer + 2), &(val = static_cast<uint64_t>(ctrl.supplyTemp.val * 1000)), 4);
  memcpy((buffer + 6), &(val = static_cast<uint64_t>(ctrl.returnTemp.val * 1000)), 4);
  memcpy((buffer + 10), &(val = static_cast<uint64_t>(ctrl.caseTemp.val * 1000)), 4);
  memcpy((buffer + 14), &(val = static_cast<uint64_t>(ctrl.aux1Temp.val * 1000)), 4);
//  memcpy((buffer + 14), &(val = static_cast<uint64_t>(ctrl.aux2Temp.val * 1000)), 4);

  memcpy((buffer + 18), &(val = static_cast<uint64_t>(ctrl.getDeltaT() * 1000)), 4);
//  memcpy((buffer + 22), &(val = static_cast<uint64_t>(ctrl.getFanPercentPID() * 1000)), 4);
//  memcpy((buffer + 26), &(val = static_cast<uint64_t>(ctrl.getFanPercentTbl() * 1000)), 4);
//  memcpy((buffer + 30), &(val = static_cast<uint64_t>(ctrl.getTempSetpoint() * 1000)), 4);

  memcpy((buffer + 34), &(rpm = ctrl.getFanRPM(0)), 2);
  memcpy((buffer + 36), &(rpm = ctrl.getFanRPM(1)), 2);
  memcpy((buffer + 38), &(rpm = ctrl.getFanRPM(2)), 2);
  memcpy((buffer + 40), &(rpm = ctrl.getFanRPM(3)), 2);
  memcpy((buffer + 42), &(rpm = ctrl.getFanRPM(4)), 2);
  memcpy((buffer + 44), &(rpm = ctrl.getFanRPM(5)), 2);

  FILL_ZEROS(buffer, 46, sizeof(buffer));
  buffer[63] = HID_DATA;  // put next state at the end
}

#endif
