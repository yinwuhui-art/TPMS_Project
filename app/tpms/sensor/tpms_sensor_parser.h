/*
 * tpms_sensor_parser.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_SENSOR_PARSER_H_
#define TPMS_SENSOR_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t sensor_id;
    TpmsWheelPos_t wheel_pos;

    int16_t pressure_kpa;
    int16_t temperature_c;

    uint16_t battery_mv;
    uint16_t status;

    int8_t rssi;
    uint32_t timestamp_ms;
} TpmsSensorFrame_t;

bool TpmsSensorParser_parse(const TpmsBleAdvReport_t *report,
                            TpmsSensorFrame_t *out_frame);

#ifdef __cplusplus
}
#endif

#endif
