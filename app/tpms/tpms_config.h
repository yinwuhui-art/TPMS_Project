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

/*
 * 四个轮胎传感器
 */
#define TPMS_WHEEL_NUM                       (4U)

/*
 * 无效传感器 ID
 */
#define TPMS_SENSOR_ID_INVALID               (0xFFFFFFFFUL)

/*
 * BLE MTU 配置
 *
 * ATT MTU = 251 byte
 * Notify 实际业务数据最大长度 = 251 - 3 = 248 byte
 */
#define TPMS_BLE_MTU_SIZE                    (251U)
#define TPMS_BLE_ATT_HEADER_SIZE             (3U)
#define TPMS_BLE_NOTIFY_MAX_LEN              (TPMS_BLE_MTU_SIZE - TPMS_BLE_ATT_HEADER_SIZE)

/*
 * 胎压、胎温无效值
 *
 * 后续内部建议统一：
 * 胎压单位：kPa
 * 胎温单位：℃
 */
#define TPMS_PRESSURE_INVALID_KPA            (-1)
#define TPMS_TEMP_INVALID_C                  (0x7FFF)


/*
 * ============================================================
 * TPMS 传感器广播过滤配置
 * ============================================================
 */

/*
 * 当前四个胎压传感器设备名称均为：
 *
 * SNP756
 *
 * ASCII:
 * S  N  P  7  5  6
 *
 * HEX:
 * 53 4E 50 37 35 36
 *
 * 作用：
 * BLEM 扫描周围 BLE 广播时，通过设备名称 SNP756
 * 判断该广播是否来自胎压传感器。
 *
 * 注意：
 * 四个传感器名称都一样，所以这个只能判断“是不是胎压传感器”，
 * 不能区分左前、右前、左后、右后。
 *
 * 后续区分轮位要靠：
 * 1. App 进入对应轮位学习
 * 2. 低频触发对应轮位传感器
 * 3. BLEM 保存该传感器 BLE 地址 / 传感器 ID
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
 * 调试用固定地址过滤配置
 * ============================================================
 */

/*
 * 调试阶段可按指定 BLE 地址过滤某一个传感器。
 *
 * 当前建议关闭：
 * 因为四个传感器都叫 SNP756，后续要做自学习，
 * 不能长期写死某一个地址。
 *
 * 1：启用指定地址过滤
 * 0：关闭指定地址过滤
 */
#define TPMS_DEBUG_FILTER_BY_SENSOR_ADDR     (0U)

/*
 * 调试用胎压传感器地址。
 *
 * nRF Connect 曾显示：
 * SNP756
 * D0:39:3F:19:83:06
 *
 * 注意：
 * TI BLE Stack 内部地址顺序可能与 nRF Connect 显示相反，
 * 所以 tpms_ble_scan.c 里会同时兼容正序和反序。
 *
 * 当前 TPMS_DEBUG_FILTER_BY_SENSOR_ADDR = 0，
 * 所以下面地址暂时不会参与过滤，只保留作调试备用。
 */
#define TPMS_DEBUG_SENSOR_ADDR_0             (0xD0U)
#define TPMS_DEBUG_SENSOR_ADDR_1             (0x39U)
#define TPMS_DEBUG_SENSOR_ADDR_2             (0x3FU)
#define TPMS_DEBUG_SENSOR_ADDR_3             (0x19U)
#define TPMS_DEBUG_SENSOR_ADDR_4             (0x83U)
#define TPMS_DEBUG_SENSOR_ADDR_5             (0x06U)


#ifdef __cplusplus
}
#endif

#endif /* TPMS_CONFIG_H_ */
