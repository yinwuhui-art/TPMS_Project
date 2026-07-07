#ifndef TPMS_APP_H_
#define TPMS_APP_H_

#include <stdint.h>
#include <stdbool.h>

#include "tpms_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void TPMS_init(void);

void TPMS_start(void);

void TPMS_stop(void);

void TPMS_periodic(uint32_t now_ms);

void TPMS_onBleAdvReport(const TpmsBleAdvReport_t *report);

void TPMS_onPhoneRxData(const uint8_t *data, uint16_t len);

const TpmsWheelData_t *TPMS_getWheelData(TpmsWheelPos_t wheel_pos);

#ifdef __cplusplus
}
#endif

#endif
