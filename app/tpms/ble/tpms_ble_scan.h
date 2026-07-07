/*
 * tpms_ble_scan.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_BLE_SCAN_H_
#define TPMS_BLE_SCAN_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void TpmsBleScan_init(void);

bool TpmsBleScan_handleAdvReport(const uint8_t *addr,
                                 uint8_t addr_type,
                                 const uint8_t *adv_data,
                                 uint16_t adv_len,
                                 int8_t rssi,
                                 uint32_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
