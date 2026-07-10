/*
 * tpms_ble_service.c
 *
 * TPMS BLE 上报接口。
 */

#include "tpms_ble_service.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/ble/profiles/simple_gatt/simple_gatt_profile.h>
#include "ti/ble/stack_util/icall/app/icall_ble_api.h"

void TpmsBleService_init(void)
{
    /*
     * GATT Service 已经由 SimpleGattProfile_addService() 注册。
     * 这里暂时不需要额外处理。
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
     * SIMPLEGATTPROFILE_CHAR2 对应 FFB2。
     *
     * FFB2 UUID:
     * 0000FFB2-0000-1000-8000-00805F9B34FB
     *
     * 属性:
     * Read / Notify
     *
     * SimpleGattProfile_setParameter() 会更新 Char2 数据，
     * 并通过 simple_gatt_profile.c 里的 Char2 CCCD 触发 Notify。
     */
    if (SimpleGattProfile_setParameter(SIMPLEGATTPROFILE_CHAR2,
                                       (uint8)len,
                                       (void *)data) == SUCCESS)
    {
        return true;
    }

    return false;
}

/*
 * 兼容旧框架接口：
 *
 * tpms_app.c 里原来调用了：
 * TpmsBleService_notifyWheelData()
 *
 * 所以这里必须保留这个函数，否则链接阶段会报：
 * unresolved symbol TpmsBleService_notifyWheelData
 *
 * 当前先不读取 TpmsWheelData_t 里的具体字段。
 * 后续等确认 tpms_types.h 里的真实字段名后，再正式打包胎压、胎温、
 * 传感器 ID、状态、故障等数据。
 */
bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data)
{
    /*
     * 临时测试上报数据：
     *
     * 0xA1：表示 TPMS 轮胎数据上报测试帧
     * 0x00：占位状态
     *
     * App 开启 FFB2 Notify 后，如果 MCU 调用到这个函数，
     * 手机 App 应该能从 FFB2 收到：A1 00
     */
    uint8_t payload[2] = {0xA1U, 0x00U};

    if (wheel_data == NULL)
    {
        return false;
    }

    return TpmsBleService_notifyFfb2(payload, sizeof(payload));
}
