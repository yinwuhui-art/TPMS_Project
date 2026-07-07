/*
 * tpms_wheel_manager.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */




#include "tpms_wheel_manager.h"
#include <stddef.h>
#include <string.h>

#include "../tpms_config.h"

static TpmsWheelData_t s_wheel_data[TPMS_WHEEL_NUM];

void TpmsWheelManager_init(void)
{
    uint8_t i;

    memset(s_wheel_data, 0, sizeof(s_wheel_data));

    for (i = 0; i < TPMS_WHEEL_NUM; i++)
    {
        s_wheel_data[i].sensor_id = TPMS_SENSOR_ID_INVALID;
        s_wheel_data[i].pressure_kpa = TPMS_PRESSURE_INVALID_KPA;
        s_wheel_data[i].temperature_c = TPMS_TEMP_INVALID_C;
        s_wheel_data[i].valid = false;
    }
}

bool TpmsWheelManager_update(const TpmsSensorFrame_t *frame)
{
    TpmsWheelData_t *wheel;

    if (frame == NULL)
    {
        return false;
    }

    if (frame->wheel_pos >= TPMS_WHEEL_NUM)
    {
        return false;
    }

    wheel = &s_wheel_data[frame->wheel_pos];

    wheel->sensor_id = frame->sensor_id;
    wheel->pressure_kpa = frame->pressure_kpa;
    wheel->temperature_c = frame->temperature_c;
    wheel->battery_mv = frame->battery_mv;
    wheel->status = frame->status;
    wheel->rssi = frame->rssi;
    wheel->last_rx_ms = frame->timestamp_ms;
    wheel->valid = true;

    return true;
}

const TpmsWheelData_t *TpmsWheelManager_get(TpmsWheelPos_t wheel_pos)
{
    if (wheel_pos >= TPMS_WHEEL_NUM)
    {
        return NULL;
    }

    return &s_wheel_data[wheel_pos];
}

void TpmsWheelManager_checkTimeout(uint32_t now_ms)
{
    uint8_t i;

    /*
     * 后面这里做传感器超时判断。
     * 比如超过 30 秒没有收到某个轮胎数据，就置故障位。
     */

    (void)now_ms;

    for (i = 0; i < TPMS_WHEEL_NUM; i++)
    {
        /* reserved */
    }
}
