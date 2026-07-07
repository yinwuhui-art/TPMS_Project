/*
 * tpms_protocol.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_PROTOCOL_H_
#define TPMS_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 手机 App 与蓝牙模块之间的临时私有协议格式：
 *
 * Byte0: 0xAA
 * Byte1: 0x55
 * Byte2: CMD
 * Byte3: LEN
 * Byte4 ~ Byte(4+LEN-1): PAYLOAD
 * Last: CHECKSUM
 *
 * CHECKSUM = CMD + LEN + PAYLOAD 所有字节低 8 位
 *
 * 后面如果客户 / App 已经有正式协议，可以只替换本文件和 tpms_protocol.c。
 */

#define TPMS_PROTOCOL_HEAD0             (0xAAU)
#define TPMS_PROTOCOL_HEAD1             (0x55U)

#define TPMS_PROTOCOL_PAYLOAD_MAX_LEN   (48U)
#define TPMS_PROTOCOL_FRAME_MAX_LEN     (5U + TPMS_PROTOCOL_PAYLOAD_MAX_LEN)

#define TPMS_PROTOCOL_CMD_QUERY_DATA    (0x01U)
#define TPMS_PROTOCOL_CMD_WAKEUP        (0x02U)
#define TPMS_PROTOCOL_CMD_START_LEARN   (0x03U)
#define TPMS_PROTOCOL_CMD_STOP_LEARN    (0x04U)
#define TPMS_PROTOCOL_CMD_CLEAR_LEARN   (0x05U)
#define TPMS_PROTOCOL_CMD_ACK           (0x80U)
#define TPMS_PROTOCOL_CMD_WHEEL_DATA    (0x81U)

typedef struct
{
    uint8_t cmd;
    uint8_t payload_len;
    uint8_t payload[TPMS_PROTOCOL_PAYLOAD_MAX_LEN];
} TpmsProtocolFrame_t;

bool TpmsProtocol_decode(const uint8_t *data,
                         uint16_t len,
                         TpmsProtocolFrame_t *out_frame);

bool TpmsProtocol_encode(uint8_t cmd,
                         const uint8_t *payload,
                         uint16_t payload_len,
                         uint8_t *out_data,
                         uint16_t out_max_len,
                         uint16_t *out_len);

bool TpmsProtocol_packOneWheelData(TpmsWheelPos_t wheel_pos,
                                   const TpmsWheelData_t *wheel_data,
                                   uint8_t *out_payload,
                                   uint16_t out_max_len,
                                   uint16_t *out_payload_len);

#ifdef __cplusplus
}
#endif

#endif
