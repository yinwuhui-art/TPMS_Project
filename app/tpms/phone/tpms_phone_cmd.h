/*
 * tpms_phone_cmd.h
 *
 * Phone APP -> BLEM FFB1 command parser
 */

#ifndef TPMS_PHONE_CMD_H_
#define TPMS_PHONE_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * ============================================================
 * FFB1：手机 APP 写入命令
 * ============================================================
 *
 * 当前已知：
 *
 * Byte0 = functionID
 *
 * 0x00：App 连接握手请求
 * 0x01：唤醒胎压传感器
 * 0x02：开始手动学习 Start_Learn_TPMS
 */
#define TPMS_PHONE_CMD_PAYLOAD_MAX_LEN       (16U)

#define TPMS_PHONE_CMD_ID_CONNECT_REQUEST    (0x00U)
#define TPMS_PHONE_CMD_ID_WAKEUP             (0x01U)
#define TPMS_PHONE_CMD_ID_START_LEARN        (0x02U)
#define TPMS_PHONE_CMD_ID_QUERY_DATA         (0x03U)
#define TPMS_PHONE_CMD_ID_STOP_LEARN         (0x04U)
#define TPMS_PHONE_CMD_ID_CLEAR_LEARN        (0x05U)


/*
 * 解析出来的命令类型
 */
typedef enum
{
    TPMS_PHONE_CMD_TYPE_NONE = 0,
    TPMS_PHONE_CMD_TYPE_CONNECT_REQUEST = 1,
    TPMS_PHONE_CMD_TYPE_WAKEUP = 2,
    TPMS_PHONE_CMD_TYPE_START_LEARN = 3,
    TPMS_PHONE_CMD_TYPE_QUERY_DATA = 4,
    TPMS_PHONE_CMD_TYPE_STOP_LEARN = 5,
    TPMS_PHONE_CMD_TYPE_CLEAR_LEARN = 6,
    TPMS_PHONE_CMD_TYPE_UNKNOWN = 255
} TpmsPhoneCmdType_t;


/*
 * TPMS 应用层处理结果
 */
typedef enum
{
    TPMS_PHONE_CMD_RESULT_NONE = 0,
    TPMS_PHONE_CMD_RESULT_CONNECT_REQUESTED = 1,
    TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED = 2,
    TPMS_PHONE_CMD_RESULT_START_LEARN = 3,
    TPMS_PHONE_CMD_RESULT_STOP_LEARN = 4,
    TPMS_PHONE_CMD_RESULT_CLEAR_LEARN = 5,
    TPMS_PHONE_CMD_RESULT_QUERY_DATA = 6,
    TPMS_PHONE_CMD_RESULT_ERROR = 255
} TpmsPhoneCmdResult_t;


typedef struct
{
    /*
     * FFB1 Byte0
     */
    uint8_t function_id;

    /*
     * 解析后的命令类型
     */
    TpmsPhoneCmdType_t type;

    /*
     * Byte1 之后的附加数据。
     */
    uint8_t payload[TPMS_PHONE_CMD_PAYLOAD_MAX_LEN];
    uint16_t payload_len;

} TpmsPhoneCmd_t;


void TpmsPhoneCmd_init(void);

bool TpmsPhoneCmd_parse(const uint8_t *data,
                        uint16_t len,
                        TpmsPhoneCmd_t *cmd);

TpmsPhoneCmdType_t TpmsPhoneCmd_getTypeByFunctionId(uint8_t function_id);

TpmsPhoneCmdResult_t TpmsPhoneCmd_handle(const TpmsPhoneCmd_t *cmd);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_PHONE_CMD_H_ */
