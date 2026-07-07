/*
 * tpms_ble_service.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */




#ifndef TPMS_BLE_SERVICE_H_
#define TPMS_BLE_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void TpmsBleService_init(void);

bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data);

void TpmsBleService_periodic(uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
