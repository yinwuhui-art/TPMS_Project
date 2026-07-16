/*
 * tpms_phone_cmd.c
 *
 * Phone APP -> BLEM FFB1 command parser
 */

#include "tpms_phone_cmd.h"

#include <string.h>


void TpmsPhoneCmd_init(void)
{
    /*
     * 当前暂时没有需要初始化的内容。
     */
}


TpmsPhoneCmdType_t TpmsPhoneCmd_getTypeByFunctionId(uint8_t function_id)
{
    TpmsPhoneCmdType_t type;

    switch (function_id)
    {
        case TPMS_PHONE_CMD_ID_WAKEUP:
            type = TPMS_PHONE_CMD_TYPE_WAKEUP;
            break;

        case TPMS_PHONE_CMD_ID_START_LEARN:
            type = TPMS_PHONE_CMD_TYPE_START_LEARN;
            break;

        case TPMS_PHONE_CMD_ID_QUERY_DATA:
            type = TPMS_PHONE_CMD_TYPE_QUERY_DATA;
            break;

        case TPMS_PHONE_CMD_ID_STOP_LEARN:
            type = TPMS_PHONE_CMD_TYPE_STOP_LEARN;
            break;

        case TPMS_PHONE_CMD_ID_CLEAR_LEARN:
            type = TPMS_PHONE_CMD_TYPE_CLEAR_LEARN;
            break;

        default:
            type = TPMS_PHONE_CMD_TYPE_UNKNOWN;
            break;
    }

    return type;
}


bool TpmsPhoneCmd_parse(const uint8_t *data,
                        uint16_t len,
                        TpmsPhoneCmd_t *cmd)
{
    uint16_t payload_len;
    uint16_t copy_len;

    if ((data == NULL) || (cmd == NULL))
    {
        return false;
    }

    if (len == 0U)
    {
        return false;
    }

    memset(cmd, 0, sizeof(TpmsPhoneCmd_t));

    /*
     * FFB1 Byte0 = functionID
     */
    cmd->function_id = data[0];
    cmd->type = TpmsPhoneCmd_getTypeByFunctionId(cmd->function_id);

    /*
     * Byte1 之后作为附加数据保存。
     */
    payload_len = (uint16_t)(len - 1U);

    if (payload_len > TPMS_PHONE_CMD_PAYLOAD_MAX_LEN)
    {
        copy_len = TPMS_PHONE_CMD_PAYLOAD_MAX_LEN;
    }
    else
    {
        copy_len = payload_len;
    }

    if (copy_len > 0U)
    {
        memcpy(cmd->payload, &data[1], copy_len);
    }

    cmd->payload_len = copy_len;

    if (cmd->type == TPMS_PHONE_CMD_TYPE_UNKNOWN)
    {
        return false;
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
        case TPMS_PHONE_CMD_TYPE_WAKEUP:
            return TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED;

        case TPMS_PHONE_CMD_TYPE_START_LEARN:
            return TPMS_PHONE_CMD_RESULT_START_LEARN;

        case TPMS_PHONE_CMD_TYPE_STOP_LEARN:
            return TPMS_PHONE_CMD_RESULT_STOP_LEARN;

        case TPMS_PHONE_CMD_TYPE_CLEAR_LEARN:
            return TPMS_PHONE_CMD_RESULT_CLEAR_LEARN;

        case TPMS_PHONE_CMD_TYPE_QUERY_DATA:
            return TPMS_PHONE_CMD_RESULT_QUERY_DATA;

        case TPMS_PHONE_CMD_TYPE_NONE:
        case TPMS_PHONE_CMD_TYPE_UNKNOWN:
        default:
            return TPMS_PHONE_CMD_RESULT_ERROR;
    }
}
