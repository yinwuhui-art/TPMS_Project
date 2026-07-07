/*
 * tpms_phone_cmd.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_PHONE_CMD_H_
#define TPMS_PHONE_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#include "tpms_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TPMS_PHONE_CMD_TYPE_UNKNOWN = 0,
    TPMS_PHONE_CMD_TYPE_QUERY_DATA,
    TPMS_PHONE_CMD_TYPE_WAKEUP,
    TPMS_PHONE_CMD_TYPE_START_LEARN,
    TPMS_PHONE_CMD_TYPE_STOP_LEARN,
    TPMS_PHONE_CMD_TYPE_CLEAR_LEARN
} TpmsPhoneCmdType_t;

typedef enum
{
    TPMS_PHONE_CMD_RESULT_NONE = 0,
    TPMS_PHONE_CMD_RESULT_QUERY_DATA,
    TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED,
    TPMS_PHONE_CMD_RESULT_START_LEARN,
    TPMS_PHONE_CMD_RESULT_STOP_LEARN,
    TPMS_PHONE_CMD_RESULT_CLEAR_LEARN,
    TPMS_PHONE_CMD_RESULT_ERROR
} TpmsPhoneCmdResult_t;

typedef struct
{
    TpmsPhoneCmdType_t type;

    uint8_t raw_cmd;

    uint8_t payload_len;
    uint8_t payload[TPMS_PROTOCOL_PAYLOAD_MAX_LEN];
} TpmsPhoneCmd_t;

void TpmsPhoneCmd_init(void);

bool TpmsPhoneCmd_parse(const uint8_t *data,
                        uint16_t len,
                        TpmsPhoneCmd_t *out_cmd);

TpmsPhoneCmdResult_t TpmsPhoneCmd_handle(const TpmsPhoneCmd_t *cmd);

#ifdef __cplusplus
}
#endif

#endif
