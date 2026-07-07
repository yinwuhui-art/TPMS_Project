/*
 * tpms_wheel_manager.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_WHEEL_MANAGER_H_
#define TPMS_WHEEL_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"
#include "../sensor/tpms_sensor_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void TpmsWheelManager_init(void);

bool TpmsWheelManager_update(const TpmsSensorFrame_t *frame);

const TpmsWheelData_t *TpmsWheelManager_get(TpmsWheelPos_t wheel_pos);

void TpmsWheelManager_checkTimeout(uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
