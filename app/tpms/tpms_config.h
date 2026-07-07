/*
 * tpms_config.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_CONFIG_H_
#define TPMS_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TPMS_WHEEL_NUM                 (4U)

#define TPMS_SENSOR_ID_INVALID         (0xFFFFFFFFUL)

#define TPMS_BLE_DEVICE_NAME           "GMW TPMS BLE"

#define TPMS_BLE_MTU_SIZE              (251U)
#define TPMS_BLE_ATT_HEADER_SIZE       (3U)
#define TPMS_BLE_NOTIFY_MAX_LEN        (TPMS_BLE_MTU_SIZE - TPMS_BLE_ATT_HEADER_SIZE)

/*
 * 后续可以根据项目要求修改：
 * 胎压单位建议内部统一用 kPa。
 */
#define TPMS_PRESSURE_INVALID_KPA      (-1)
#define TPMS_TEMP_INVALID_C            (0x7FFF)

#ifdef __cplusplus
}
#endif

#endif
