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

/*
 * ============================================================
 * TPMS 基础配置
 * ============================================================
 */

#define TPMS_WHEEL_NUM                       (4U)

#define TPMS_SENSOR_ID_INVALID               (0xFFFFFFFFUL)

/*
 * ATT MTU = 251 byte
 * Notify 实际业务数据最大长度 = 251 - 3 = 248 byte
 */
#define TPMS_BLE_MTU_SIZE                    (251U)
#define TPMS_BLE_ATT_HEADER_SIZE             (3U)
#define TPMS_BLE_NOTIFY_MAX_LEN              (TPMS_BLE_MTU_SIZE - TPMS_BLE_ATT_HEADER_SIZE)

#define TPMS_PRESSURE_INVALID_KPA            (-1)
#define TPMS_TEMP_INVALID_C                  (0x7FFF)


/*
 * ============================================================
 * TPMS 传感器名称配置
 * ============================================================
 *
 * 四个胎压传感器名称均为：
 *
 * SNP756
 *
 * HEX:
 * 53 4E 50 37 35 36
 */

#define TPMS_FILTER_BY_DEVICE_NAME           (1U)

#define TPMS_TARGET_NAME_LEN                 (6U)

#define TPMS_TARGET_NAME_BYTE_0              (0x53U)    /* S */
#define TPMS_TARGET_NAME_BYTE_1              (0x4EU)    /* N */
#define TPMS_TARGET_NAME_BYTE_2              (0x50U)    /* P */
#define TPMS_TARGET_NAME_BYTE_3              (0x37U)    /* 7 */
#define TPMS_TARGET_NAME_BYTE_4              (0x35U)    /* 5 */
#define TPMS_TARGET_NAME_BYTE_5              (0x36U)    /* 6 */


/*
 * ============================================================
 * TPMS 双重过滤配置：名称 + 四个物理地址
 * ============================================================
 *
 * 当前工程实现：
 *
 * 1. 地址白名单是主过滤条件。
 * 2. SNP756 名称是验证条件。
 *
 * 原因：
 * TPMS 广播包不一定每一包都带 Local Name。
 * 名称 SNP756 可能出现在 Scan Response 中。
 * 如果强制要求同一包同时满足“名称 + 地址”，容易漏掉其他传感器。
 *
 * 地址按 nRF Connect 显示顺序填写。
 *
 * 例如：
 * nRF 显示 D0:39:3F:19:83:06
 *
 * 就填写：
 * D0 39 3F 19 83 06
 *
 * tpms_ble_scan.c 内部会同时兼容 TI BLE Stack 反序地址：
 * 06 83 19 3F 39 D0
 */

#define TPMS_FILTER_BY_ADDR_WHITELIST        (1U)

#define TPMS_SENSOR_ADDR_NUM                 (4U)

/*
 * index 0：LF 左前
 * MAC: D0:39:3F:19:83:06
 */
#define TPMS_SENSOR_LF_ADDR_0                (0xD0U)
#define TPMS_SENSOR_LF_ADDR_1                (0x39U)
#define TPMS_SENSOR_LF_ADDR_2                (0x3FU)
#define TPMS_SENSOR_LF_ADDR_3                (0x19U)
#define TPMS_SENSOR_LF_ADDR_4                (0x83U)
#define TPMS_SENSOR_LF_ADDR_5                (0x06U)

/*
 * index 1：RF 右前
 * MAC: D0:39:3F:18:68:D3
 */
#define TPMS_SENSOR_RF_ADDR_0                (0xD0U)
#define TPMS_SENSOR_RF_ADDR_1                (0x39U)
#define TPMS_SENSOR_RF_ADDR_2                (0x3FU)
#define TPMS_SENSOR_RF_ADDR_3                (0x18U)
#define TPMS_SENSOR_RF_ADDR_4                (0x68U)
#define TPMS_SENSOR_RF_ADDR_5                (0xD3U)

/*
 * index 2：RR 右后
 * MAC: D0:39:3F:19:27:31
 */
#define TPMS_SENSOR_RR_ADDR_0                (0xD0U)
#define TPMS_SENSOR_RR_ADDR_1                (0x39U)
#define TPMS_SENSOR_RR_ADDR_2                (0x3FU)
#define TPMS_SENSOR_RR_ADDR_3                (0x19U)
#define TPMS_SENSOR_RR_ADDR_4                (0x27U)
#define TPMS_SENSOR_RR_ADDR_5                (0x31U)

/*
 * index 3：LR 左后
 * MAC: D0:39:3F:17:BA:01
 */
#define TPMS_SENSOR_LR_ADDR_0                (0xD0U)
#define TPMS_SENSOR_LR_ADDR_1                (0x39U)
#define TPMS_SENSOR_LR_ADDR_2                (0x3FU)
#define TPMS_SENSOR_LR_ADDR_3                (0x17U)
#define TPMS_SENSOR_LR_ADDR_4                (0xBAU)
#define TPMS_SENSOR_LR_ADDR_5                (0x01U)


#ifdef __cplusplus
}
#endif

#endif /* TPMS_CONFIG_H_ */
