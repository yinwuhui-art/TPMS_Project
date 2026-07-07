/*
 * tpms_fault.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_FAULT_H_
#define TPMS_FAULT_H_

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TPMS_FAULT_NONE              (0x0000U)
#define TPMS_FAULT_LOW_PRESSURE      (0x0001U)
#define TPMS_FAULT_HIGH_PRESSURE     (0x0002U)
#define TPMS_FAULT_HIGH_TEMP         (0x0004U)
#define TPMS_FAULT_LOW_BATTERY       (0x0008U)
#define TPMS_FAULT_SENSOR_TIMEOUT    (0x0010U)
#define TPMS_FAULT_SENSOR_INVALID    (0x0020U)

typedef struct
{
    int16_t low_pressure_kpa;
    int16_t high_pressure_kpa;
    int16_t high_temperature_c;
    uint16_t low_battery_mv;
    uint32_t sensor_timeout_ms;
} TpmsFaultThreshold_t;

void TpmsFault_init(void);

void TpmsFault_setThreshold(const TpmsFaultThreshold_t *threshold);

uint16_t TpmsFault_evaluateWheel(const TpmsWheelData_t *wheel_data,
                                 uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
