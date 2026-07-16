/*
 * tpms_ble_scan.h
 *
 * TPMS 传感器 BLE 广播扫描处理
 */

#ifndef TPMS_BLE_SCAN_H_
#define TPMS_BLE_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * 无串口调试时，在 CCS Expressions 里观察这些变量。
 */
extern volatile uint32_t g_tpms_scan_adv_count;
extern volatile uint32_t g_tpms_scan_tpms_count;
extern volatile int8_t   g_tpms_scan_last_rssi;
extern volatile uint8_t  g_tpms_scan_last_addr[6];
extern volatile uint8_t  g_tpms_scan_last_addr_type;
extern volatile uint8_t  g_tpms_scan_last_data[64];
extern volatile uint8_t  g_tpms_scan_last_len;
extern volatile uint8_t  g_tpms_scan_last_is_tpms;
extern volatile uint32_t g_tpms_scan_last_timestamp_ms;

/*
 * 专门保存最近一次识别到的胎压传感器广播。
 * 不会被周围其他 BLE 设备覆盖。
 */
extern volatile int8_t   g_tpms_scan_last_tpms_rssi;
extern volatile uint8_t  g_tpms_scan_last_tpms_addr[6];
extern volatile uint8_t  g_tpms_scan_last_tpms_addr_type;
extern volatile uint8_t  g_tpms_scan_last_tpms_data[64];
extern volatile uint8_t  g_tpms_scan_last_tpms_len;



/*
 * 调试用：保存扫描过程中 RSSI 最强的一包广播。
 * 用于确认 BLEM 实际扫到的传感器地址和广播数据。
 */
extern volatile int8_t   g_tpms_scan_best_rssi;
extern volatile uint8_t  g_tpms_scan_best_addr[6];
extern volatile uint8_t  g_tpms_scan_best_addr_type;
extern volatile uint8_t  g_tpms_scan_best_data[64];
extern volatile uint8_t  g_tpms_scan_best_len;

void TpmsBleScan_init(void);
/*
 * 处理 TI Observer 扫描到的广播包。
 *
 * addr         : 广播设备地址
 * addr_type    : 地址类型
 * data         : 广播数据
 * data_len     : 广播数据长度
 * rssi         : 信号强度
 * timestamp_ms : 时间戳，当前可先传 0
 */
bool TpmsBleScan_handleAdvReport(const uint8_t *addr,
                                 uint8_t addr_type,
                                 const uint8_t *data,
                                 uint8_t data_len,
                                 int8_t rssi,
                                 uint32_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_BLE_SCAN_H_ */
