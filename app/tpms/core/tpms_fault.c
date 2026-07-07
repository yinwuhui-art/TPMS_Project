/*
 * tpms_fault.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */



#include "tpms_fault.h"

#include <stddef.h>
#include <string.h>

static TpmsFaultThreshold_t s_threshold;

void TpmsFault_init(void)
{
    /*
     * 这些阈值只是工程默认值，后面需要根据项目需求调整。
     *
     * 胎压单位：kPa
     * 温度单位：摄氏度
     * 电池单位：mV
     */
    s_threshold.low_pressure_kpa = 180;
    s_threshold.high_pressure_kpa = 350;
    s_threshold.high_temperature_c = 85;
    s_threshold.low_battery_mv = 2200;
    s_threshold.sensor_timeout_ms = 30000UL;
}

void TpmsFault_setThreshold(const TpmsFaultThreshold_t *threshold)
{
    if (threshold == NULL)
    {
        return;
    }

    memcpy(&s_threshold, threshold, sizeof(TpmsFaultThreshold_t));
}

uint16_t TpmsFault_evaluateWheel(const TpmsWheelData_t *wheel_data,
                                 uint32_t now_ms)
{
    uint16_t fault = TPMS_FAULT_NONE;
    uint32_t elapsed_ms;

    if (wheel_data == NULL)
    {
        return TPMS_FAULT_SENSOR_INVALID;
    }

    if (wheel_data->valid == false)
    {
        return TPMS_FAULT_SENSOR_INVALID;
    }

    if (wheel_data->pressure_kpa < s_threshold.low_pressure_kpa)
    {
        fault |= TPMS_FAULT_LOW_PRESSURE;
    }

    if (wheel_data->pressure_kpa > s_threshold.high_pressure_kpa)
    {
        fault |= TPMS_FAULT_HIGH_PRESSURE;
    }

    if (wheel_data->temperature_c > s_threshold.high_temperature_c)
    {
        fault |= TPMS_FAULT_HIGH_TEMP;
    }

    if ((wheel_data->battery_mv > 0U) &&
        (wheel_data->battery_mv < s_threshold.low_battery_mv))
    {
        fault |= TPMS_FAULT_LOW_BATTERY;
    }

    if (now_ms >= wheel_data->last_rx_ms)
    {
        elapsed_ms = now_ms - wheel_data->last_rx_ms;
    }
    else
    {
        /*
         * 简单处理 tick 回绕。
         */
        elapsed_ms = 0xFFFFFFFFUL - wheel_data->last_rx_ms + now_ms + 1UL;
    }

    if (elapsed_ms > s_threshold.sensor_timeout_ms)
    {
        fault |= TPMS_FAULT_SENSOR_TIMEOUT;
    }

    return fault;
}
