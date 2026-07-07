/*
 * tpms_ble_wakeup.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_BLE_WAKEUP_H_
#define TPMS_BLE_WAKEUP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TPMS_BLE_WAKEUP_STATE_IDLE = 0,
    TPMS_BLE_WAKEUP_STATE_REQUESTED,
    TPMS_BLE_WAKEUP_STATE_SENDING,
    TPMS_BLE_WAKEUP_STATE_DONE,
    TPMS_BLE_WAKEUP_STATE_ERROR
} TpmsBleWakeupState_t;

void TpmsBleWakeup_init(void);

bool TpmsBleWakeup_request(void);

void TpmsBleWakeup_periodic(uint32_t now_ms);

TpmsBleWakeupState_t TpmsBleWakeup_getState(void);

bool TpmsBleWakeup_isPending(void);

#ifdef __cplusplus
}
#endif

#endif
