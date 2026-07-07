/*
 * tpms_sensor_parser.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */




#include "tpms_sensor_parser.h"
#include <stddef.h>
#include <string.h>

bool TpmsSensorParser_parse(const TpmsBleAdvReport_t *report,
                            TpmsSensorFrame_t *out_frame)
{
    if ((report == NULL) || (out_frame == NULL))
    {
        return false;
    }

    /*
     * 这里暂时不真正解析。
     *
     * 后面等你确认四个胎压传感器的 BLE 广播数据格式之后，
     * 再在这里解析：
     *
     * 1. Sensor ID
     * 2. Pressure
     * 3. Temperature
     * 4. Battery
     * 5. Status / Fault
     * 6. Wheel position / learning result
     */

    memset(out_frame, 0, sizeof(TpmsSensorFrame_t));

    out_frame->sensor_id = 0xFFFFFFFFUL;
    out_frame->wheel_pos = TPMS_WHEEL_UNKNOWN;
    out_frame->pressure_kpa = -1;
    out_frame->temperature_c = 0x7FFF;
    out_frame->battery_mv = 0;
    out_frame->status = 0;
    out_frame->rssi = report->rssi;
    out_frame->timestamp_ms = report->timestamp_ms;

    return false;
}
