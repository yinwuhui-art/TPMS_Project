#include "tpms_app.h"

#include <stddef.h>

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

static bool s_tpms_started = false;

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
                TpmsBleService_notifyWheelData(wheel);
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
        case TPMS_PHONE_CMD_RESULT_QUERY_DATA:
            /*
             * 手机 App 查询当前四轮数据。
             * 当前先逐轮调用 notifyWheelData。
             * 后面会改成 tpms_protocol.c 打包四轮完整数据，再通过 FFB2 Notify。
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

        case TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED:
            /*
             * 唤醒请求已经进入 tpms_ble_wakeup 状态机。
             */
            break;

        case TPMS_PHONE_CMD_RESULT_START_LEARN:
            /*
             * 后面接入学习流程。
             */
            break;

        case TPMS_PHONE_CMD_RESULT_STOP_LEARN:
            /*
             * 后面停止学习流程。
             */
            break;

        case TPMS_PHONE_CMD_RESULT_CLEAR_LEARN:
            /*
             * 后面清除学习结果。
             */
            break;

        case TPMS_PHONE_CMD_RESULT_ERROR:
        case TPMS_PHONE_CMD_RESULT_NONE:
        default:
            break;
    }
}

const TpmsWheelData_t *TPMS_getWheelData(TpmsWheelPos_t wheel_pos)
{
    return TpmsWheelManager_get(wheel_pos);
}
