/*
 * tpms_ble_scan.h
 *
 * TPMS sensor BLE advertisement scan handler
 */

#ifndef TPMS_BLE_SCAN_H_
#define TPMS_BLE_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * ============================================================
 * TPMS 扫描匹配原因
 * ============================================================
 */

#define TPMS_SCAN_MATCH_NONE                 (0U)
#define TPMS_SCAN_MATCH_BY_NAME              (1U)
#define TPMS_SCAN_MATCH_BY_ADDR              (2U)
#define TPMS_SCAN_MATCH_BY_NAME_AND_ADDR     (3U)

/*
 * ============================================================
 * TPMS 地址匹配方式
 * ============================================================
 *
 * 0 = 未匹配
 * 1 = 完整地址正序匹配
 * 2 = 完整地址反序匹配
 * 3 = 后三字节正序匹配
 * 4 = 后三字节反序匹配
 */

#define TPMS_SCAN_ADDR_MATCH_NONE            (0U)
#define TPMS_SCAN_ADDR_MATCH_FULL_NORMAL     (1U)
#define TPMS_SCAN_ADDR_MATCH_FULL_REVERSE    (2U)
#define TPMS_SCAN_ADDR_MATCH_SUFFIX_NORMAL   (3U)
#define TPMS_SCAN_ADDR_MATCH_SUFFIX_REVERSE  (4U)

/*
 * ============================================================
 * TPMS 扫描抓包缓存配置
 * ============================================================
 */

#define TPMS_SCAN_CAPTURE_LOG_SIZE           (16U)
#define TPMS_SCAN_CAPTURE_DATA_MAX_LEN       (64U)
#define TPMS_SCAN_CAPTURE_MFR_MAX_LEN        (32U)


/*
 * ============================================================
 * 最近一包任意 BLE 广播
 * ============================================================
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
 * ============================================================
 * 最近一包识别到的 TPMS 广播
 * ============================================================
 */

extern volatile int8_t   g_tpms_scan_last_tpms_rssi;
extern volatile uint8_t  g_tpms_scan_last_tpms_addr[6];
extern volatile uint8_t  g_tpms_scan_last_tpms_addr_type;
extern volatile uint8_t  g_tpms_scan_last_tpms_data[64];
extern volatile uint8_t  g_tpms_scan_last_tpms_len;


/*
 * ============================================================
 * 当前扫描过程中 RSSI 最强的一包广播
 * ============================================================
 */

extern volatile int8_t   g_tpms_scan_best_rssi;
extern volatile uint8_t  g_tpms_scan_best_addr[6];
extern volatile uint8_t  g_tpms_scan_best_addr_type;
extern volatile uint8_t  g_tpms_scan_best_data[64];
extern volatile uint8_t  g_tpms_scan_best_len;


/*
 * ============================================================
 * 名称匹配统计
 * ============================================================
 */

extern volatile uint32_t g_tpms_scan_name_match_count;


/*
 * ============================================================
 * 地址白名单匹配统计
 * ============================================================
 */

extern volatile uint32_t g_tpms_scan_addr_whitelist_match_count;
extern volatile uint32_t g_tpms_scan_name_and_addr_match_count;

extern volatile uint8_t  g_tpms_scan_last_whitelist_index;
extern volatile uint8_t  g_tpms_scan_last_name_verified;
extern volatile uint8_t  g_tpms_scan_last_addr_match_mode;

/*
 * 名称是 SNP756，但是地址没有命中白名单的调试记录。
 *
 * 用途：
 * 判断是不是 BLEM 已经扫到了 SNP756，
 * 但是 MAC 地址配置或者地址顺序没有匹配上。
 */
extern volatile uint32_t g_tpms_scan_name_only_count;
extern volatile uint8_t  g_tpms_scan_name_only_last_addr[6];

/*
 * 四个白名单地址命中次数
 *
 * index 0 = LF 左前
 * index 1 = RF 右前
 * index 2 = RR 右后
 * index 3 = LR 左后
 */
extern volatile uint32_t g_tpms_scan_white_addr_hit_count[4];
extern volatile uint8_t  g_tpms_scan_white_name_verified[4];
extern volatile uint8_t  g_tpms_scan_white_last_addr[4][6];


/*
 * ============================================================
 * 最近一次 TPMS 匹配原因
 * ============================================================
 */

extern volatile uint8_t g_tpms_scan_last_match_reason;


/*
 * ============================================================
 * 最近一次 TPMS Manufacturer Data
 * ============================================================
 */

extern volatile uint8_t g_tpms_scan_last_mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN];
extern volatile uint8_t g_tpms_scan_last_mfr_len;


/*
 * ============================================================
 * 最近 16 包 TPMS 原始广播缓存
 * ============================================================
 */

extern volatile uint32_t g_tpms_capture_total_count;
extern volatile uint8_t  g_tpms_capture_write_index;

extern volatile uint8_t  g_tpms_capture_match_reason[TPMS_SCAN_CAPTURE_LOG_SIZE];
extern volatile int8_t   g_tpms_capture_rssi[TPMS_SCAN_CAPTURE_LOG_SIZE];
extern volatile uint8_t  g_tpms_capture_addr_type[TPMS_SCAN_CAPTURE_LOG_SIZE];
extern volatile uint8_t  g_tpms_capture_addr[TPMS_SCAN_CAPTURE_LOG_SIZE][6];

extern volatile uint8_t  g_tpms_capture_len[TPMS_SCAN_CAPTURE_LOG_SIZE];
extern volatile uint8_t  g_tpms_capture_data[TPMS_SCAN_CAPTURE_LOG_SIZE][TPMS_SCAN_CAPTURE_DATA_MAX_LEN];

extern volatile uint8_t  g_tpms_capture_mfr_len[TPMS_SCAN_CAPTURE_LOG_SIZE];
extern volatile uint8_t  g_tpms_capture_mfr_data[TPMS_SCAN_CAPTURE_LOG_SIZE][TPMS_SCAN_CAPTURE_MFR_MAX_LEN];


/*
 * ============================================================
 * 当前抓到的最长 Manufacturer Data
 * ============================================================
 */

extern volatile uint8_t  g_tpms_capture_max_mfr_len;
extern volatile uint8_t  g_tpms_capture_max_mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN];
extern volatile uint8_t  g_tpms_capture_max_mfr_addr[6];
extern volatile int8_t   g_tpms_capture_max_mfr_rssi;


/*
 * ============================================================
 * 对外接口
 * ============================================================
 */

void TpmsBleScan_init(void);

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
