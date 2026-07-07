/*
 * tpms_ble_service.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */



#include "tpms_ble_service.h"
#include <stddef.h>
void TpmsBleService_init(void)
{
    /*
     * 后面这里对接 common/simple_gatt_profile.c
     * 或者你自己新增的 TPMS GATT Service。
     */
}

bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data)
{
    if (wheel_data == NULL)
    {
        return false;
    }

    /*
     * 后面这里把四轮数据打包后，
     * 通过 UUID FFB2 Notify 给手机 APP。
     */

    return true;
}

void TpmsBleService_periodic(uint32_t now_ms)
{
    (void)now_ms;
}
