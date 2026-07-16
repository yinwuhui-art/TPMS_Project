#include "tpms_app.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "tpms_config.h"

#include "core/tpms_fault.h"
#include "core/tpms_localization.h"
#include "core/tpms_wheel_manager.h"

#include "sensor/tpms_sensor_parser.h"

#include "ble/tpms_ble_service.h"
#include "ble/tpms_ble_scan.h"
#include "ble/tpms_ble_wakeup.h"

#include "storage/tpms_storage.h"

#include "phone/tpms_phone_cmd.h"
#include "phone/tpms_protocol.h"


static bool s_tpms_started = false;


/*
 * 本文件内部测试函数声明。
 *
 * 作用：
 * 手机 APP 往 FFB1 写 01 或 02 后，
 * BLEM 先通过 FFB2 回复一帧正式 12 字节测试报文。
 */
static void TpmsApp_sendTestFfb2Report(uint8_t position);


void TPMS_init(void)
{
    TpmsStorage_init();

    TpmsFault_init();
    TpmsLocalization_init();
    TpmsWheelManager_init();

    TpmsBleService_init();
    TpmsBleScan_init();
    TpmsBleWakeup_init();

    TpmsPhoneCmd_init();

    s_tpms_started = false;
}


void TPMS_start(void)
{
    s_tpms_started = true;
}


void TPMS_stop(void)
{
    s_tpms_started = false;
}


void TPMS_periodic(uint32_t now_ms)
{
    if (s_tpms_started == false)
    {
        return;
    }

    TpmsWheelManager_checkTimeout(now_ms);
    TpmsBleService_periodic(now_ms);
    TpmsBleWakeup_periodic(now_ms);
}


void TPMS_onBleAdvReport(const TpmsBleAdvReport_t *report)
{
    TpmsSensorFrame_t frame;

    if (s_tpms_started == false)
    {
        return;
    }

    if (report == NULL)
    {
        return;
    }

    if (TpmsSensorParser_parse(report, &frame) == true)
    {
        /*
         * 传感器解析出来后，先做轮位判断。
         *
         * 如果广播数据里面已经带有轮位，TpmsLocalization_onSensorFrame()
         * 会直接返回 true。
         *
         * 如果广播数据里面只有 sensor_id，则通过绑定表 / 自定位算法判断轮位。
         */
        if (TpmsLocalization_onSensorFrame(&frame) == false)
        {
            return;
        }

        if (TpmsWheelManager_update(&frame) == true)
        {
            const TpmsWheelData_t *wheel;

            wheel = TpmsWheelManager_get(frame.wheel_pos);

            if (wheel != NULL)
            {
                /*
                 * 注意：
                 * TpmsBleService_notifyWheelData() 现在应该已经被改成
                 * 正式 FFB2 12 字节格式，不再发送 A1/A2 临时报文。
                 */
                (void)TpmsBleService_notifyWheelData(wheel);
            }
        }
    }
}


void TPMS_onPhoneRxData(const uint8_t *data, uint16_t len)
{
    TpmsPhoneCmd_t cmd;
    TpmsPhoneCmdResult_t result;
    uint8_t i;

    if (s_tpms_started == false)
    {
        return;
    }

    if ((data == NULL) || (len == 0U))
    {
        return;
    }

    if (TpmsPhoneCmd_parse(data, len, &cmd) == false)
    {
        return;
    }

    result = TpmsPhoneCmd_handle(&cmd);

    switch (result)
    {
        case TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED:
        {
            /*
             * 手机 APP 写 FFB1 = 01
             *
             * 当前先回复一帧正式 12 字节测试报文。
             *
             * 期望手机 APP / nRF Connect 收到：
             *
             * 00 01 12 34 56 78 50 5A 25 FF FF 00
             *
             * 最后一个字节 00 表示未知轮位。
             */
            TpmsApp_sendTestFfb2Report(TPMS_APP_POS_NONE);
            break;
        }

        case TPMS_PHONE_CMD_RESULT_START_LEARN:
        {
            /*
             * 手机 APP 写 FFB1 = 02
             *
             * 当前先模拟左前 LF 学习结果。
             *
             * 期望手机 APP / nRF Connect 收到：
             *
             * 00 01 12 34 56 78 50 5A 25 FF FF 01
             *
             * 最后一个字节 01 表示 LF 左前轮。
             */
            TpmsApp_sendTestFfb2Report(TPMS_APP_POS_LF);
            break;
        }

        case TPMS_PHONE_CMD_RESULT_QUERY_DATA:
        {
            /*
             * 手机 APP 查询当前四轮数据。
             *
             * 当前先逐轮调用 notifyWheelData。
             * 后续真正解析胎压传感器数据后，再把四轮真实数据打包发送。
             */
            for (i = 0U; i < TPMS_WHEEL_NUM; i++)
            {
                const TpmsWheelData_t *wheel;

                wheel = TpmsWheelManager_get((TpmsWheelPos_t)i);

                if (wheel != NULL)
                {
                    (void)TpmsBleService_notifyWheelData(wheel);
                }
            }

            break;
        }

        case TPMS_PHONE_CMD_RESULT_STOP_LEARN:
        {
            /*
             * 后续这里接停止学习流程。
             */
            break;
        }

        case TPMS_PHONE_CMD_RESULT_CLEAR_LEARN:
        {
            /*
             * 后续这里接清除学习结果流程。
             */
            break;
        }

        case TPMS_PHONE_CMD_RESULT_ERROR:
        case TPMS_PHONE_CMD_RESULT_NONE:
        default:
        {
            break;
        }
    }
}


const TpmsWheelData_t *TPMS_getWheelData(TpmsWheelPos_t wheel_pos)
{
    return TpmsWheelManager_get(wheel_pos);
}


static void TpmsApp_sendTestFfb2Report(uint8_t position)
{
    uint8_t sensor_id[4] = {0x12U, 0x34U, 0x56U, 0x78U};
    uint8_t info;

    /*
     * Byte8 Info:
     *
     * 厂家：森萨塔 001
     * 模式：运行模式 001
     * 电池：正常 0
     * 低频：已触发 1
     *
     * Byte8 = 001 001 0 1
     * Byte8 = 0x25
     */
    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_RUNNING,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_TRIGGERED);

    /*
     * 正式 FFB2 12 字节格式：
     *
     * Byte0  = 00
     * Byte1  = 01
     * Byte2  = 12
     * Byte3  = 34
     * Byte4  = 56
     * Byte5  = 78
     * Byte6  = 50
     * Byte7  = 5A
     * Byte8  = 25
     * Byte9  = FF
     * Byte10 = FF
     * Byte11 = position
     */
    (void)TpmsBleService_notifyFfb2SensorRaw(sensor_id,
                                            0x50U,
                                            0x5AU,
                                            info,
                                            TPMS_APP_FUNC_REUSE_DEFAULT_0,
                                            TPMS_APP_FUNC_REUSE_DEFAULT_1,
                                            position);
}
