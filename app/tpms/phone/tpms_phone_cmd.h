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
 * 0x01：唤醒胎压传感器
 * 0x02：开始手动学习 Start_Learn_TPMS
 *
 * 后续如果 APP 协议增加其他命令，再继续扩展。
 */
#define TPMS_PHONE_CMD_PAYLOAD_MAX_LEN       (16U)

#define TPMS_PHONE_CMD_ID_WAKEUP             (0x01U)
#define TPMS_PHONE_CMD_ID_START_LEARN        (0x02U)
#define TPMS_PHONE_CMD_ID_QUERY_DATA         (0x03U)
#define TPMS_PHONE_CMD_ID_STOP_LEARN         (0x04U)
#define TPMS_PHONE_CMD_ID_CLEAR_LEARN        (0x05U)


typedef enum
{
    TPMS_PHONE_CMD_TYPE_NONE = 0,
    TPMS_PHONE_CMD_TYPE_WAKEUP,
    TPMS_PHONE_CMD_TYPE_START_LEARN,
    TPMS_PHONE_CMD_TYPE_QUERY_DATA,
    TPMS_PHONE_CMD_TYPE_STOP_LEARN,
    TPMS_PHONE_CMD_TYPE_CLEAR_LEARN,
    TPMS_PHONE_CMD_TYPE_UNKNOWN
} TpmsPhoneCmdType_t;

/*
 * ============================================================
 * 兼容 tpms_app.c 的命令处理结果
 * ============================================================
 *
 * 注意：
 * 这个 Result 是给 tpms_app.c 用的。
 * TpmsPhoneCmdType_t 表示“解析出来的命令类型”；
 * TpmsPhoneCmdResult_t 表示“TPMS 应用层处理后的结果”。
 */
typedef enum
{
    TPMS_PHONE_CMD_RESULT_NONE = 0,
    TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED,
    TPMS_PHONE_CMD_RESULT_START_LEARN,
    TPMS_PHONE_CMD_RESULT_STOP_LEARN,
    TPMS_PHONE_CMD_RESULT_CLEAR_LEARN,
    TPMS_PHONE_CMD_RESULT_QUERY_DATA,
    TPMS_PHONE_CMD_RESULT_ERROR
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
     *
     * 当前 0x01、0x02 暂时只用 Byte0，
     * 所以 payload_len 通常为 0。
     */
    uint8_t payload[TPMS_PHONE_CMD_PAYLOAD_MAX_LEN];
    uint16_t payload_len;

} TpmsPhoneCmd_t;


/*
 * 初始化命令模块。
 */
void TpmsPhoneCmd_init(void);

/*
 * 解析手机 APP 写入 FFB1 的数据。
 */
bool TpmsPhoneCmd_parse(const uint8_t *data,
                        uint16_t len,
                        TpmsPhoneCmd_t *cmd);

/*
 * 根据 functionID 获取命令类型。
 */
TpmsPhoneCmdType_t TpmsPhoneCmd_getTypeByFunctionId(uint8_t function_id);

/*
 * 兼容 tpms_app.c 的旧接口。
 *
 * 直接处理手机 APP 写入 FFB1 的数据，
 * 返回 TPMS 应用层处理结果。
 */
TpmsPhoneCmdResult_t TpmsPhoneCmd_handle(const TpmsPhoneCmd_t *cmd);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_PHONE_CMD_H_ */
