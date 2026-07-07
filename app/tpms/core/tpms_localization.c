/*
 * tpms_localization.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */


#include "tpms_localization.h"

#include <stddef.h>

#include "../tpms_config.h"

static uint32_t s_sensor_id_table[TPMS_WHEEL_NUM];

void TpmsLocalization_init(void)
{
    TpmsLocalization_clear();
}

void TpmsLocalization_clear(void)
{
    uint8_t i;

    for (i = 0U; i < TPMS_WHEEL_NUM; i++)
    {
        s_sensor_id_table[i] = TPMS_SENSOR_ID_INVALID;
    }
}

bool TpmsLocalization_bindSensor(uint32_t sensor_id,
                                 TpmsWheelPos_t wheel_pos)
{
    if (wheel_pos >= TPMS_WHEEL_NUM)
    {
        return false;
    }

    if (sensor_id == TPMS_SENSOR_ID_INVALID)
    {
        return false;
    }

    s_sensor_id_table[wheel_pos] = sensor_id;

    return true;
}

TpmsWheelPos_t TpmsLocalization_resolve(uint32_t sensor_id,
                                        int8_t rssi)
{
    uint8_t i;

    (void)rssi;

    if (sensor_id == TPMS_SENSOR_ID_INVALID)
    {
        return TPMS_WHEEL_UNKNOWN;
    }

    /*
     * 第一阶段：通过已学习 / 已绑定的 Sensor ID 判断轮位。
     *
     * 后面真正做四轮自定位算法时，可以在这里结合：
     * 1. RSSI
     * 2. 接收顺序
     * 3. 轮速 / 车辆信号
     * 4. 自定位算法包输出结果
     */
    for (i = 0U; i < TPMS_WHEEL_NUM; i++)
    {
        if (s_sensor_id_table[i] == sensor_id)
        {
            return (TpmsWheelPos_t)i;
        }
    }

    return TPMS_WHEEL_UNKNOWN;
}

bool TpmsLocalization_onSensorFrame(TpmsSensorFrame_t *frame)
{
    TpmsWheelPos_t wheel_pos;

    if (frame == NULL)
    {
        return false;
    }

    if (frame->wheel_pos < TPMS_WHEEL_NUM)
    {
        return true;
    }

    wheel_pos = TpmsLocalization_resolve(frame->sensor_id, frame->rssi);

    if (wheel_pos >= TPMS_WHEEL_NUM)
    {
        return false;
    }

    frame->wheel_pos = wheel_pos;

    return true;
}

