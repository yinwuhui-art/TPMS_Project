/*
 * tpms_ble_service.c
 */

#include "tpms_ble_service.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/ble/profiles/simple_gatt/simple_gatt_profile.h>
#include "ti/ble/stack_util/icall/app/icall_ble_api.h"


void TpmsBleService_init(void)
{
    /*
     * 当前暂时没有单独初始化内容。
     */
}


bool TpmsBleService_notifyFfb2(const uint8_t *data, uint16_t len)
{
    if ((data == NULL) || (len == 0U))
    {
        return false;
    }

    if (len > SIMPLEGATTPROFILE_CHAR2_MAX_LEN)
    {
        return false;
    }

    /*
     * SIMPLEGATTPROFILE_CHAR2 对应 FFB2：
     *
     * UUID: 0000FFB2-0000-1000-8000-00805F9B34FB
     * 属性: Notify + Read
     */
    if (SimpleGattProfile_setParameter(SIMPLEGATTPROFILE_CHAR2,
                                       (uint8)len,
                                       (void *)data) == SUCCESS)
    {
        return true;
    }

    return false;
}


bool TpmsBleService_notifyFfb2AppReport(const TpmsAppFfb2Report_t *report)
{
    uint8_t payload[TPMS_APP_FFB2_REPORT_LEN];

    if (TpmsProtocol_buildFfb2Report(report,
                                     payload,
                                     sizeof(payload)) != true)
    {
        return false;
    }

    return TpmsBleService_notifyFfb2(payload, TPMS_APP_FFB2_REPORT_LEN);
}


bool TpmsBleService_notifyFfb2SensorRaw(const uint8_t sensor_id[4],
                                        uint8_t pressure_raw,
                                        uint8_t temperature_raw,
                                        uint8_t info,
                                        uint8_t function_reuse_0,
                                        uint8_t function_reuse_1,
                                        uint8_t position)
{
    TpmsAppFfb2Report_t report;

    if (sensor_id == NULL)
    {
        return false;
    }

    memset(&report, 0, sizeof(report));

    report.sensor_id[0] = sensor_id[0];
    report.sensor_id[1] = sensor_id[1];
    report.sensor_id[2] = sensor_id[2];
    report.sensor_id[3] = sensor_id[3];

    report.pressure_raw = pressure_raw;
    report.temperature_raw = temperature_raw;
    report.info = info;

    report.function_reuse[0] = function_reuse_0;
    report.function_reuse[1] = function_reuse_1;

    report.position = position;

    return TpmsBleService_notifyFfb2AppReport(&report);
}


bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data)
{
    /*
     * 兼容旧接口：
     *
     * 之前这里可能发过 A1 00 之类的临时调试帧。
     * 现在禁止继续发临时格式，统一改成正式 12 字节格式。
     *
     * 由于当前 TpmsWheelData_t 的具体字段你之前没有完全固定，
     * 这里先不强行访问 wheel_data->xxx，避免再次出现：
     *
     * no member named 'wheel_pos'
     * no member named 'fault_flags'
     *
     * 后续等 TpmsWheelData_t 字段确定后，再把真实字段映射进来。
     */
    uint8_t sensor_id[4] = {0x00U, 0x00U, 0x00U, 0x00U};
    uint8_t info;

    if (wheel_data == NULL)
    {
        return false;
    }

    /*
     * 默认 Info：
     *
     * 厂家：森萨塔 001
     * 模式：运行模式 001
     * 电池：正常 0
     * 低频：非低频触发 0
     *
     * Byte8 = 001 001 0 0 = 0x24
     */
    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_RUNNING,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_NOT_TRIGGERED);

    /*
     * 先发一帧正式格式占位数据：
     *
     * Byte0~1  = 00 01
     * Byte2~5  = 00 00 00 00
     * Byte6    = 00
     * Byte7    = 50，对应 0℃
     * Byte8    = info
     * Byte9~10 = FF FF
     * Byte11   = 00，未知轮位
     */
    return TpmsBleService_notifyFfb2SensorRaw(sensor_id,
                                             0x00U,
                                             50U,
                                             info,
                                             TPMS_APP_FUNC_REUSE_DEFAULT_0,
                                             TPMS_APP_FUNC_REUSE_DEFAULT_1,
                                             TPMS_APP_POS_NONE);
}
