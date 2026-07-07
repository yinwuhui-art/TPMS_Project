/*
 * tpms_localization.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_LOCALIZATION_H_
#define TPMS_LOCALIZATION_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"
#include "../sensor/tpms_sensor_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void TpmsLocalization_init(void);

bool TpmsLocalization_bindSensor(uint32_t sensor_id,
                                 TpmsWheelPos_t wheel_pos);

TpmsWheelPos_t TpmsLocalization_resolve(uint32_t sensor_id,
                                        int8_t rssi);

bool TpmsLocalization_onSensorFrame(TpmsSensorFrame_t *frame);

void TpmsLocalization_clear(void);

#ifdef __cplusplus
}
#endif

#endif
