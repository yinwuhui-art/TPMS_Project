/*
 * tpms_types.h
 *
 * TPMS common data types
 */

#ifndef TPMS_TYPES_H_
#define TPMS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


/*
 * ============================================================
 * 轮位索引定义
 * ============================================================
 *
 * 注意：
 * 这里是内部数组索引，不是 FFB2 Byte11。
 *
 * 内部索引：
 * LF = 0
 * RF = 1
 * RR = 2
 * LR = 3
 *
 * FFB2 Byte11：
 * LF = 0x01
 * RF = 0x02
 * RR = 0x08
 * LR = 0x04
 */

typedef enum
{
    TPMS_WHEEL_LF      = 0U,
    TPMS_WHEEL_RF      = 1U,
    TPMS_WHEEL_RR      = 2U,
    TPMS_WHEEL_LR      = 3U,
    TPMS_WHEEL_COUNT   = 4U,
    TPMS_WHEEL_UNKNOWN = 0xFFU
} TpmsWheelPos_t;


/*
 * 为了兼容之前代码里可能出现的名字
 */
typedef TpmsWheelPos_t TpmsWheelPosition_t;


/*
 * ============================================================
 * FFB2 Byte11 轮位 bit 定义
 * ============================================================
 *
 * 这些值用于发给 App。
 */

#define TPMS_WHEEL_POS_NONE                  (0x00U)
#define TPMS_WHEEL_POS_LF                    (0x01U)
#define TPMS_WHEEL_POS_RF                    (0x02U)
#define TPMS_WHEEL_POS_LR                    (0x04U)
#define TPMS_WHEEL_POS_RR                    (0x08U)


/*
 * ============================================================
 * BLE 广播数据结构
 * ============================================================
 *
 * 用于兼容 tpms_app.h / app_observer.c 里可能引用的类型。
 */

#define TPMS_BLE_ADV_DATA_MAX_LEN            (64U)

typedef struct
{
    uint8_t  addr[6];
    uint8_t  addr_type;

    int8_t   rssi;

    uint8_t  data[TPMS_BLE_ADV_DATA_MAX_LEN];
    uint8_t  data_len;

    uint32_t timestamp_ms;

    bool     valid;
} TpmsBleAdvReport_t;


/*
 * ============================================================
 * TPMS 传感器原始帧
 * ============================================================
 *
 * 用于：
 * 1. tpms_sensor_parser
 * 2. tpms_wheel_manager
 * 3. tpms_localization
 */

typedef struct
{
    uint32_t sensor_id;

    TpmsWheelPos_t wheel_pos;

    uint16_t pressure_kpa;
    int16_t  temperature_c;

    uint8_t  pressure_raw;
    uint8_t  temperature_raw;

    uint8_t  status;
    uint8_t  status_raw;

    uint16_t battery_mv;

    uint16_t fault_flags;

    int8_t   rssi;

    uint32_t timestamp_ms;
    uint32_t last_rx_ms;

    uint8_t  addr[6];
    uint8_t  addr_type;

    uint8_t  function_reuse[2];

    bool     valid;
} TpmsSensorFrame_t;


/*
 * ============================================================
 * TPMS 轮位数据
 * ============================================================
 *
 * 用于：
 * 1. tpms_wheel_manager
 * 2. tpms_fault
 * 3. tpms_ble_service
 */

typedef struct
{
    uint32_t sensor_id;

    TpmsWheelPos_t wheel_pos;

    uint16_t pressure_kpa;
    int16_t  temperature_c;

    uint8_t  pressure_raw;
    uint8_t  temperature_raw;

    uint8_t  status;
    uint8_t  status_raw;

    uint16_t battery_mv;

    uint16_t fault_flags;

    int8_t   rssi;

    uint32_t last_update_ms;
    uint32_t last_rx_ms;

    uint8_t  addr[6];
    uint8_t  addr_type;

    uint8_t  function_reuse[2];

    bool     valid;
} TpmsWheelData_t;


#ifdef __cplusplus
}
#endif

#endif /* TPMS_TYPES_H_ */
