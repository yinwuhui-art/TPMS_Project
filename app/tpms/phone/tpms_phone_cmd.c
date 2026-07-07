/*
 * tpms_phone_cmd.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#include "tpms_phone_cmd.h"

#include <stddef.h>
#include <string.h>

#include "../ble/tpms_ble_wakeup.h"

void TpmsPhoneCmd_init(void)
{
    /*
     * 目前没有内部状态。
     * 后面可以在这里初始化手机命令状态机。
     */
}

bool TpmsPhoneCmd_parse(const uint8_t *data,
                        uint16_t len,
                        TpmsPhoneCmd_t *out_cmd)
{
    TpmsProtocolFrame_t frame;

    if ((data == NULL) || (out_cmd == NULL))
    {
        return false;
    }

    if (TpmsProtocol_decode(data, len, &frame) == false)
    {
        return false;
    }

    memset(out_cmd, 0, sizeof(TpmsPhoneCmd_t));

    out_cmd->raw_cmd = frame.cmd;
    out_cmd->payload_len = frame.payload_len;

    if (frame.payload_len > 0U)
    {
        memcpy(out_cmd->payload, frame.payload, frame.payload_len);
    }

    switch (frame.cmd)
    {
        case TPMS_PROTOCOL_CMD_QUERY_DATA:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_QUERY_DATA;
            break;

        case TPMS_PROTOCOL_CMD_WAKEUP:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_WAKEUP;
            break;

        case TPMS_PROTOCOL_CMD_START_LEARN:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_START_LEARN;
            break;

        case TPMS_PROTOCOL_CMD_STOP_LEARN:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_STOP_LEARN;
            break;

        case TPMS_PROTOCOL_CMD_CLEAR_LEARN:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_CLEAR_LEARN;
            break;

        default:
            out_cmd->type = TPMS_PHONE_CMD_TYPE_UNKNOWN;
            break;
    }

    return true;
}

TpmsPhoneCmdResult_t TpmsPhoneCmd_handle(const TpmsPhoneCmd_t *cmd)
{
    if (cmd == NULL)
    {
        return TPMS_PHONE_CMD_RESULT_ERROR;
    }

    switch (cmd->type)
    {
        case TPMS_PHONE_CMD_TYPE_QUERY_DATA:
            return TPMS_PHONE_CMD_RESULT_QUERY_DATA;

        case TPMS_PHONE_CMD_TYPE_WAKEUP:
            if (TpmsBleWakeup_request() == true)
            {
                return TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED;
            }
            return TPMS_PHONE_CMD_RESULT_ERROR;

        case TPMS_PHONE_CMD_TYPE_START_LEARN:
            /*
             * 后面接入学习流程。
             */
            return TPMS_PHONE_CMD_RESULT_START_LEARN;

        case TPMS_PHONE_CMD_TYPE_STOP_LEARN:
            /*
             * 后面停止学习流程。
             */
            return TPMS_PHONE_CMD_RESULT_STOP_LEARN;

        case TPMS_PHONE_CMD_TYPE_CLEAR_LEARN:
            /*
             * 后面清除学习结果。
             */
            return TPMS_PHONE_CMD_RESULT_CLEAR_LEARN;

        default:
            break;
    }

    return TPMS_PHONE_CMD_RESULT_ERROR;
}


