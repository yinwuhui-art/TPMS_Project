/*
 * tpms_types.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_TYPES_H_
#define TPMS_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TPMS_WHEEL_FL = 0,      /* Front Left  左前 */
    TPMS_WHEEL_FR = 1,      /* Front Right 右前 */
    TPMS_WHEEL_RL = 2,      /* Rear Left   左后 */
    TPMS_WHEEL_RR = 3,      /* Rear Right  右后 */

    TPMS_WHEEL_UNKNOWN = 0xFF
} TpmsWheelPos_t;

typedef struct
{
    uint32_t sensor_id;

    int16_t pressure_kpa;
    int16_t temperature_c;

    uint16_t battery_mv;
    uint16_t status;

    int8_t rssi;
    uint32_t last_rx_ms;

    bool valid;
} TpmsWheelData_t;

typedef struct
{
    const uint8_t *data;
    uint16_t len;

    uint8_t addr[6];
    uint8_t addr_type;

    int8_t rssi;
    uint32_t timestamp_ms;
} TpmsBleAdvReport_t;

#ifdef __cplusplus
}
#endif

#endif
