//
// Created by jd on 11/26/2019.
//

#include <memory>

#include "pb_encode.h"
#include "pb_decode.h"

#include "runtime_config.h"
#include "runtime_config_v1.pb.h"


#define DEFAULT_PERCENT_TABLE {{26, 1}, {26,1}, {26, 1}, {26, 1}, {26, 1}, {26, 1}, {26, 1}, {26, 28}, {30, 65}, {33, 100}}
#define ZERO_PERCENT_TABLE {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}

#define DEFAULT_PID {\
    24,\
    75,\
    100,\
    28.0f,\
    28.0f,\
    31.5f,\
    34,\
    0.9f,\
    0.02f,\
    true,\
    true,\
    0.5f,\
    {45, 60, 2.1f},\
    {65, 30, 1.8f}\
  }


/*
   Macros for nanopb encode/decode errors
*/
#ifdef ARDUINO
#define PRINT_STREAM_ERR(from, stream) { Serial.print(from); Serial.print(": "); Serial.println(PB_GET_ERROR(stream)); }
#else
#define PRINT_STREAM_ERR(from, stream) qDebug() << from << ": " << PB_GET_ERROR(stream)
#endif


// reserve memory ahead of time for various config structs
static tfcproto_RuntimeConfigV1 config;
static tfcproto_TableConfigV1 tbl_out_v1 = tfcproto_TableConfigV1_init_zero;
static RuntimeConfig::TableConfig tbl_out = {ZERO_PERCENT_TABLE};


RuntimeConfig::RuntimeConfig()
    : config_version{CONTROLLER_VERSION},
      fan1{4, 5, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
      fan2{6, 7, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 0.8f, {DEFAULT_PERCENT_TABLE}},
      fan3{10, 11, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
      fan4{9, 8, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 0.8f, {DEFAULT_PERCENT_TABLE}},
      fan5{3, 2, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
      fan6{22, 12, CONTROL_MODE::MODE_PID, CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP, 1, {DEFAULT_PERCENT_TABLE}},
      tempSupply{A7, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
      tempReturn{A6, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
      tempCase{A4, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
      tempAux1{0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID},
      tempAux2{0, DEFAULT_BCOEFFICIENT, DEFAULT_SERIESRESISTOR, DEFAULT_THERMISTORNOMINAL, DEFAULT_PID}
{
}

RuntimeConfig::RuntimeConfig(uint8_t config_version,
                             FanConfig fan1,
                             FanConfig fan2,
                             FanConfig fan3,
                             FanConfig fan4,
                             FanConfig fan5,
                             FanConfig fan6,
                             SensorConfig tempSupply,
                             SensorConfig tempReturn,
                             SensorConfig tempCase,
                             SensorConfig tempAux1,
                             SensorConfig tempAux2)
    : config_version{config_version},
      fan1{fan1}, fan2{fan2}, fan3{fan3}, fan4{fan4}, fan5{fan5}, fan6{fan6},
      tempSupply{tempSupply}, tempReturn{tempReturn}, tempCase{tempCase}, tempAux1{tempAux1}, tempAux2{tempAux2}
{
}

int RuntimeConfig::toBytes(byte *bytes, const uint16_t len) const
{
  if (len < CONFIG_BYTES) {
#ifdef ARDUINO
    Serial.println("Logic Error: CONFIG_BYTES bytes needed for program configuration");
#endif
    return -1;
  }

  memset(bytes, '\0', len);  // null all bytes

  bytes[CONFIG_POS_VERSION] = CONTROLLER_VERSION;
  bytes[CONFIG_POS_KEY1] = CONFIG_KEY1;
  bytes[CONFIG_POS_KEY2] = CONFIG_KEY2;

  // convert RuntimeConfiguration into a protobuf struct
  config = toProto();
  // Create a stream that will write to config bytes
  pb_ostream_t stream = pb_ostream_from_buffer((bytes + CONFIG_POS_CONFIG), (len - CONFIG_POS_CONFIG - 1));
  // encode config message into bytes
  bool status = pb_encode(&stream, tfcproto_RuntimeConfigV1_fields, &config);
  if (!status) {
    PRINT_STREAM_ERR("RuntimeConfig::toBytes(), encoding failed", &stream);
    return -1;
  }

  // Note: do not need to check length, as encode fails if not enough storage
  const size_t &configLen = stream.bytes_written;
  // write config message length
  memcpy((bytes + CONFIG_POS_CONFIG_LENGTH), &configLen, 2);

  // log config length
#ifdef ARDUINO
  Serial.print("RuntimeConfig::toBytes() configLength = ");
  Serial.println(configLen);
#else
  qDebug() << "RuntimeConfig::toBytes() configLength =" << configLen;
#endif

  return 0;
}

/*static */RuntimeConfig RuntimeConfig::parse_bytes(const byte bytes[], const uint16_t len)
{
  if (len < CONFIG_BYTES) {
#ifdef ARDUINO
    Serial.println("Logic Error: CONFIG_BYTES needed to parse program configuration, defaults returned");
#endif
    return RuntimeConfig();  // Return defaults
  }
  else if (static_cast<uint8_t>(bytes[CONFIG_POS_VERSION]) != CONTROLLER_VERSION || static_cast<uint8_t>(bytes[CONFIG_POS_VERSION]) < 1 || bytes[CONFIG_POS_KEY1] != CONFIG_KEY1 || bytes[CONFIG_POS_KEY2] != CONFIG_KEY2) {
#ifdef ARDUINO
    if (static_cast<uint8_t>(bytes[CONFIG_POS_VERSION]) != 0) {
      Serial.println("Config version mismatch, defaults returned");
    }
#endif
    return RuntimeConfig();  // Return defaults
  }

  // read config message length
  uint16_t configLen = 0;
  memcpy(&configLen, (bytes + CONFIG_POS_CONFIG_LENGTH), 2);
  if (configLen >= len - CONFIG_POS_CONFIG) {
#ifdef ARDUINO
    Serial.print("Config length (");
    Serial.print(configLen);
    Serial.println(") exceeds supported size, defaults returned");
#else
    qDebug() << "Config length (" <<  configLen << ") exceeds supported size, defaults returned";
#endif
    return RuntimeConfig();  // Return defaults
  }

  // parse into struct
  config = tfcproto_RuntimeConfigV1_init_zero;

  // Create a stream that reads from the buffer.
  pb_istream_t stream = pb_istream_from_buffer((bytes + CONFIG_POS_CONFIG), configLen + 1);

  // decode message
  bool status = pb_decode(&stream, tfcproto_RuntimeConfigV1_fields, &config);
  if (!status) {
    PRINT_STREAM_ERR("RuntimeConfig::parse_bytes(), decoding failed", &stream);
    return RuntimeConfig();  // Return defaults
  }
  return from_proto(config);
}


CONTROL_MODE CONTROL_MODE_from_proto(const tfcproto_ControlMode &in)
{
  switch (in) {
    case tfcproto_ControlMode_MODE_TBL:
      return CONTROL_MODE::MODE_TBL;
    case tfcproto_ControlMode_MODE_PID:
      return CONTROL_MODE::MODE_PID;
    case tfcproto_ControlMode_MODE_FIXED:
      return CONTROL_MODE::MODE_FIXED;
    case tfcproto_ControlMode_MODE_OFF:
      return CONTROL_MODE::MODE_OFF;
  }
  return CONTROL_MODE::MODE_OFF;
}

tfcproto_ControlMode CONTROL_MODE_to_proto(CONTROL_MODE in)
{
  switch (in) {
    case CONTROL_MODE::MODE_TBL:
      return tfcproto_ControlMode_MODE_TBL;
    case CONTROL_MODE::MODE_PID:
      return tfcproto_ControlMode_MODE_PID;
    case CONTROL_MODE::MODE_FIXED:
      return tfcproto_ControlMode_MODE_FIXED;
    case CONTROL_MODE::MODE_OFF:
      return tfcproto_ControlMode_MODE_OFF;
  }
  return tfcproto_ControlMode_MODE_OFF;
}


CONTROL_SOURCE CONTROL_SOURCE_from_proto(const tfcproto_ControlSource &in)
{
  switch (in) {
    case tfcproto_ControlSource_SENSOR_WATER_SUPPLY_TEMP:
      return CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP;
    case tfcproto_ControlSource_SENSOR_WATER_RETURN_TEMP:
      return CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP;
    case tfcproto_ControlSource_SENSOR_CASE_TEMP:
      return CONTROL_SOURCE::SENSOR_CASE_TEMP;
    case tfcproto_ControlSource_SENSOR_AUX1_TEMP:
      return CONTROL_SOURCE::SENSOR_AUX1_TEMP;
    case tfcproto_ControlSource_SENSOR_AUX2_TEMP:
      return CONTROL_SOURCE::SENSOR_AUX2_TEMP;
    case tfcproto_ControlSource_VIRTUAL_DELTA_TEMP:
      return CONTROL_SOURCE::VIRTUAL_DELTA_TEMP;
  }
  return CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP;
}

tfcproto_ControlSource CONTROL_SOURCE_to_proto(CONTROL_SOURCE in)
{
  switch (in) {
    case CONTROL_SOURCE::SENSOR_WATER_SUPPLY_TEMP:
      return tfcproto_ControlSource_SENSOR_WATER_SUPPLY_TEMP;
    case CONTROL_SOURCE::SENSOR_WATER_RETURN_TEMP:
      return tfcproto_ControlSource_SENSOR_WATER_RETURN_TEMP;
    case CONTROL_SOURCE::SENSOR_CASE_TEMP:
      return tfcproto_ControlSource_SENSOR_CASE_TEMP;
    case CONTROL_SOURCE::SENSOR_AUX1_TEMP:
      return tfcproto_ControlSource_SENSOR_AUX1_TEMP;
    case CONTROL_SOURCE::SENSOR_AUX2_TEMP:
      return tfcproto_ControlSource_SENSOR_AUX2_TEMP;
    case CONTROL_SOURCE::VIRTUAL_DELTA_TEMP:
      return tfcproto_ControlSource_VIRTUAL_DELTA_TEMP;
  }
  return tfcproto_ControlSource_SENSOR_WATER_SUPPLY_TEMP;
}


tfcproto_RuntimeConfigV1 RuntimeConfig::toProto() const
{
  return {
      static_cast<uint32_t>(config_version),
      /*true, */fan1.toProto(),
      /*true, */fan2.toProto(),
      /*true, */fan3.toProto(),
      /*true, */fan4.toProto(),
      /*true, */fan5.toProto(),
      /*true, */fan6.toProto(),
      /*true, */tempSupply.toProto(),
      /*true, */tempReturn.toProto(),
      /*true, */tempCase.toProto(),
      /*true, */tempAux1.toProto(),
      /*true, */tempAux2.toProto()
  };
}

/*static */RuntimeConfig RuntimeConfig::from_proto(tfcproto_RuntimeConfigV1 &in)
{
  return {
      static_cast<uint8_t>(in.config_version),
      FanConfig::from_proto(in.fan1),
      FanConfig::from_proto(in.fan2),
      FanConfig::from_proto(in.fan3),
      FanConfig::from_proto(in.fan4),
      FanConfig::from_proto(in.fan5),
      FanConfig::from_proto(in.fan6),
      SensorConfig::from_proto(in.temp_supply),
      SensorConfig::from_proto(in.temp_return),
      SensorConfig::from_proto(in.temp_case),
      SensorConfig::from_proto(in.temp_aux1),
      SensorConfig::from_proto(in.temp_aux2)
  };
}


tfcproto_FanConfigV1 RuntimeConfig::FanConfig::toProto() const
{
  return {
      pinPWM,
      pinRPM,
      CONTROL_MODE_to_proto(mode),
      CONTROL_SOURCE_to_proto(source),
      static_cast<uint32_t>(ratio * 100),
      /*true, */tbl.toProto()
  };
}

/*static */RuntimeConfig::FanConfig RuntimeConfig::FanConfig::from_proto(tfcproto_FanConfigV1 &in)
{
  return {
      static_cast<uint8_t>(in.pin_pwm),
      static_cast<uint8_t>(in.pin_rpm),
      in.pin_pwm ? CONTROL_MODE_from_proto(in.mode) : CONTROL_MODE::MODE_OFF,  // force mode OFF if no PWM pin mapped
      CONTROL_SOURCE_from_proto(in.source),
      static_cast<float>(in.ratio / 100.0),
      TableConfig::from_proto(in.tbl)
  };
}


// for non fixed size percent tables (decode fails w/ error end-of-stream):
//struct TableConfigWrapper {
//  RuntimeConfig::TableConfig tbl;
//  uint8_t cur;
//};
//// callback for reading from TableConfigV1.entries
//bool tfcproto_TableConfigV1_decode_single_entry(pb_istream_t *istream, const pb_field_t *field, void **arg)
//{
//  TableConfigWrapper *dest = (TableConfigWrapper * )(*arg);
//  // decode single entry
//  tfcproto_TableConfigV1_TableEntryV1 entry;
//  if (!pb_decode(istream, tfcproto_TableConfigV1_TableEntryV1_fields, &entry)) {
//    PRINT_STREAM_ERR("tfcproto_TableConfigV1_decode_single_entry, decode failed", istream);
//    return false;
//  }
//#ifdef ARDUINO
//  Serial.println(String("decode: entry.temp: ") + entry.temp + ", entry.pct: " + entry.pct);
//#else
//  qDebug() << "decode: entry.temp: " << entry.temp << ", entry.pct: " << entry.pct;
//#endif
//  // add to dest tbl
//  if (dest->cur < FAN_TBL_SIZE) {
//    auto &tbl = dest->tbl.temp_pct_table;
//    tbl[dest->cur][0] = entry.temp;
//    tbl[dest->cur][1] = entry.pct;
//    dest->cur++;
//    return true;
//  }
//  else {
//    return false;
//  }
//}
//// callback for writing to TableConfigV1.entries
//bool tfcproto_TableConfigV1_encode_entries(pb_ostream_t *ostream, const pb_field_t *field, void * const *arg)
//{
//  RuntimeConfig::TableConfig *source = (RuntimeConfig::TableConfig * )(*arg);
//  tfcproto_TableConfigV1_TableEntryV1 entry = tfcproto_TableConfigV1_TableEntryV1_init_zero;
//  const auto &tempPctTbl = source->temp_pct_table;
//  uint8_t i;
//  for (i = 0; i < FAN_TBL_SIZE; i++) {
//    entry = {
//        static_cast<uint32_t>(tempPctTbl[i][0] * 500),
//        static_cast<uint32_t>(tempPctTbl[i][1] * 500)
//    };
//#ifdef ARDUINO
////    Serial.println(String("encode: entry.temp: ") + entry.temp + ", entry.pct: " + entry.pct);
//#else
//    qDebug() << "encode: entry.temp: " << entry.temp << ", entry.pct: " << entry.pct;
//#endif
//    if (!pb_encode_tag_for_field(ostream, field)) {
//      PRINT_STREAM_ERR("tfcproto_TableConfigV1_encode_entries pb_encode_tag_for_field, encode failed", ostream);
//      return false;
//    }
//    if (!pb_encode(ostream, tfcproto_TableConfigV1_TableEntryV1_fields, &entry)) {
//      PRINT_STREAM_ERR("tfcproto_TableConfigV1_encode_entries pb_encode_submessage, encode failed", ostream);
//      return false;
//    }
//  }
//  return true;
//}

tfcproto_TableConfigV1 RuntimeConfig::TableConfig::toProto() const
{
  tfcproto_TableConfigV1 &out = tbl_out_v1;

  auto &entries = out.entries;
  uint8_t i;
  for (i = 0; i < FAN_TBL_SIZE; i++) {
    entries[i].temp = static_cast<uint32_t>(temp_pct_table[i][0] * 500);
    entries[i].pct = static_cast<uint32_t>(temp_pct_table[i][1] * 500);
  }
  return out;
  // for non fixed size percent tables (decode fails w/ error end-of-stream):
//  // non-const copy
//  RuntimeConfig::TableConfig copy = *this;
//  tfcproto_TableConfigV1 out = tfcproto_TableConfigV1_init_zero;
//  out.entries.arg = &copy;
//  out.entries.funcs.encode = tfcproto_TableConfigV1_encode_entries;
//  byte buffer[1024];
//  memset(buffer, '\0', 1024);
//  pb_ostream_t stream = pb_ostream_from_buffer(buffer, 1024);
//  if (!pb_encode(&stream, tfcproto_TableConfigV1_fields, &out)) {
//    PRINT_STREAM_ERR("TableConfig::toProto, encode failed", &stream);
//    return {{{NULL}, NULL}};
//  }
//#ifdef ARDUINO
//  Serial.println(String("TableConfig bytes encoded: ") + stream.bytes_written);
//#else
//  qDebug() << "TableConfig bytes encoded: " << stream.bytes_written;
//#endif
//  return out;
}

/*static */RuntimeConfig::TableConfig RuntimeConfig::TableConfig::from_proto(tfcproto_TableConfigV1 &in)
{
  RuntimeConfig::TableConfig &out = tbl_out;

  auto &tbl = out.temp_pct_table;
  const auto &entries = in.entries;
  uint8_t i;
  for (i = 0; i < FAN_TBL_SIZE; i++) {
    tbl[i][0] = static_cast<float>(entries[i].temp / 500.0);
    tbl[i][1] = static_cast<float>(entries[i].pct / 500.0);
  }
  return out;
  // for non fixed size percent tables (decode fails w/ error end-of-stream):
//  TableConfigWrapper dest = {
//      {ZERO_PERCENT_TABLE},
//      0
//  };
//  byte buffer[1024];
//  memset(buffer, '\0', 1024);
//  // determine length of table config message
//  size_t len = 0;
//  pb_get_encoded_size(&len, tfcproto_TableConfigV1_fields, &in);
//  // prepare the nanopb DECODING callback
//  in.entries.arg = &dest;
//  in.entries.funcs.decode = tfcproto_TableConfigV1_decode_single_entry;
//  pb_istream_t stream = pb_istream_from_buffer(buffer, len + 1);
//  if (!pb_decode(&stream, tfcproto_TableConfigV1_fields, &in)) {
//    PRINT_STREAM_ERR("TableConfig::from_proto, decode failed", &stream);
//    return {DEFAULT_PERCENT_TABLE};
//  }
//#ifdef ARDUINO
//  Serial.println(String("TableConfig bytes decoded: ") + (len - stream.bytes_left));
//#else
//  qDebug() << "TableConfig bytes decoded: " <<  (len - stream.bytes_left);
//#endif
//  return dest.tbl;
}


tfcproto_SensorConfigV1 RuntimeConfig::SensorConfig::toProto() const
{
  return {
      pin,
      beta,
      seriesR,
      nominalR,
      /*true, */pid.toProto()
  };
}

/*static */RuntimeConfig::SensorConfig RuntimeConfig::SensorConfig::from_proto(tfcproto_SensorConfigV1 &in)
{
  return {
      static_cast<uint8_t>(in.pin),
      static_cast<uint16_t>(in.beta),
      static_cast<uint16_t>(in.series_r),
      static_cast<uint16_t>(in.nominal_r),
      PIDConfig::from_proto(in.pid)
  };
}


tfcproto_PIDConfigV1 RuntimeConfig::PIDConfig::toProto() const
{
  return {
      pwm_percent_min,
      pwm_percent_max1,
      pwm_percent_max2,
      static_cast<uint32_t>(setpoint * 10),
      static_cast<uint32_t>(setpoint_min * 10),
      static_cast<uint32_t>(setpoint_max * 10),
      gain_p,
      static_cast<uint32_t>(gain_i * 100),
      static_cast<uint32_t>(gain_d * 100),
      adaptive_sp,
      adaptive_sp_check_case_temp,
      static_cast<uint32_t>(adaptive_sp_step_size * 100),
      /*true, */adaptive_sp_step_down.toProto(),
      /*true, */adaptive_sp_step_up.toProto()
  };
}

/*static */RuntimeConfig::PIDConfig RuntimeConfig::PIDConfig::from_proto(tfcproto_PIDConfigV1 &in)
{
  return {
      static_cast<uint8_t>(in.pwm_percent_min),
      static_cast<uint8_t>(in.pwm_percent_max1),
      static_cast<uint8_t>(in.pwm_percent_max2),
      static_cast<float>(in.setpoint / 10.0),
      static_cast<float>(in.setpoint_min / 10.0),
      static_cast<float>(in.setpoint_max / 10.0),
      static_cast<uint8_t>(in.gain_p),
      static_cast<float>(in.gain_i / 100.0),
      static_cast<float>(in.gain_d / 100.0),
      in.adaptive_sp,
      in.adaptive_sp_check_case_temp,
      static_cast<float>(in.adaptive_sp_step_size / 100.0),
      PIDStep::from_proto(in.adaptive_sp_step_down),
      PIDStep::from_proto(in.adaptive_sp_step_up)
  };
}


tfcproto_PIDStepV1 RuntimeConfig::PIDConfig::PIDStep::toProto() const
{
  return {
      pct,
      delay,
      static_cast<uint32_t>(case_temp_delta * 100)
  };
}

/*static */RuntimeConfig::PIDConfig::PIDStep RuntimeConfig::PIDConfig::PIDStep::from_proto(tfcproto_PIDStepV1 &in)
{
  return {
      static_cast<uint8_t>(in.pct),
      static_cast<uint16_t>(in.delay),
      static_cast<float>(in.case_temp_delta / 100.0)
  };
}
