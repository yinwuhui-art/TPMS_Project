/*
 * tpms_protocol.h
 *
 * BLEM -> Phone APP FFB2 protocol
 */

#ifndef TPMS_PROTOCOL_H_
#define TPMS_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * ============================================================
 * FFB2 报文长度
 * ============================================================
 *
 * 按当前协议表：
 *
 * Byte0  ~ Byte1   固定值 0x00 0x01
 * Byte2  ~ Byte5   传感器 ID
 * Byte6            Pressure
 * Byte7            Temperature
 * Byte8            Info
 * Byte9  ~ Byte10  FunctionReuse
 * Byte11           position
 */
#define TPMS_APP_FFB2_REPORT_LEN             (12U)

/*
 * Byte 下标
 */
#define TPMS_APP_FFB2_IDX_FIXED_0            (0U)
#define TPMS_APP_FFB2_IDX_FIXED_1            (1U)
#define TPMS_APP_FFB2_IDX_SENSOR_ID_0        (2U)
#define TPMS_APP_FFB2_IDX_SENSOR_ID_1        (3U)
#define TPMS_APP_FFB2_IDX_SENSOR_ID_2        (4U)
#define TPMS_APP_FFB2_IDX_SENSOR_ID_3        (5U)
#define TPMS_APP_FFB2_IDX_PRESSURE           (6U)
#define TPMS_APP_FFB2_IDX_TEMPERATURE        (7U)
#define TPMS_APP_FFB2_IDX_INFO               (8U)
#define TPMS_APP_FFB2_IDX_FUNC_REUSE_0       (9U)
#define TPMS_APP_FFB2_IDX_FUNC_REUSE_1       (10U)
#define TPMS_APP_FFB2_IDX_POSITION           (11U)

/*
 * Byte0 ~ Byte1 固定值
 */
#define TPMS_APP_FFB2_FIXED_BYTE0            (0x00U)
#define TPMS_APP_FFB2_FIXED_BYTE1            (0x01U)


/*
 * ============================================================
 * Byte8：Info 字段
 * ============================================================
 *
 * 图中 Bit8~Bit6：厂家
 * 图中 Bit5~Bit3：模式
 * 图中 Bit2：电池
 * 图中 Bit1：低频触发
 *
 * C 语言中：
 *
 * Bit1 -> bit0
 * Bit2 -> bit1
 * Bit3 -> bit2
 * Bit4 -> bit3
 * Bit5 -> bit4
 * Bit6 -> bit5
 * Bit7 -> bit6
 * Bit8 -> bit7
 */

/*
 * 厂家 vendor，占 Byte8 bit7~bit5
 */
#define TPMS_APP_VENDOR_SENSATA              (0x01U)    /* 001：森萨塔 */
#define TPMS_APP_VENDOR_YUNLING              (0x02U)    /* 010：云岭 */
#define TPMS_APP_VENDOR_SHIANTE              (0x03U)    /* 011：驶安特 */
#define TPMS_APP_VENDOR_BAOZHANG             (0x04U)    /* 100：保隆 */

/*
 * 模式 mode，占 Byte8 bit4~bit2
 */
#define TPMS_APP_MODE_SLEEP                  (0x00U)    /* 000：休眠模式 */
#define TPMS_APP_MODE_RUNNING                (0x01U)    /* 001：运行模式 */
#define TPMS_APP_MODE_PARKING                (0x02U)    /* 010：停车模式 */
#define TPMS_APP_MODE_LOCALIZATION           (0x03U)    /* 011：定位模式 */
#define TPMS_APP_MODE_PAL_SERVICE            (0x04U)    /* 100：PAL Service */
#define TPMS_APP_MODE_ROLL_SERVICE           (0x05U)    /* 101：Roll Service */

/*
 * 电池状态，占 Byte8 bit1
 */
#define TPMS_APP_BATTERY_NORMAL              (0x00U)
#define TPMS_APP_BATTERY_LOW                 (0x01U)

/*
 * 低频触发状态，占 Byte8 bit0
 */
#define TPMS_APP_LF_NOT_TRIGGERED            (0x00U)
#define TPMS_APP_LF_TRIGGERED                (0x01U)


/*
 * ============================================================
 * Byte11：position 字段
 * ============================================================
 *
 * 图中：
 *
 * Bit4 = RR
 * Bit3 = LR
 * Bit2 = RF
 * Bit1 = LF
 *
 * C 语言中：
 *
 * Bit1 -> bit0 -> 0x01
 * Bit2 -> bit1 -> 0x02
 * Bit3 -> bit2 -> 0x04
 * Bit4 -> bit3 -> 0x08
 */
#define TPMS_APP_POS_NONE                    (0x00U)
#define TPMS_APP_POS_LF                      (0x01U)    /* 左前 LF */
#define TPMS_APP_POS_RF                      (0x02U)    /* 右前 RF */
#define TPMS_APP_POS_LR                      (0x04U)    /* 左后 LR */
#define TPMS_APP_POS_RR                      (0x08U)    /* 右后 RR */


/*
 * ============================================================
 * FunctionReuse 默认值
 * ============================================================
 *
 * 当前还没有完整解析 Byte9~Byte10 的所有复用含义时，
 * 先按保留位填 0xFF 0xFF。
 *
 * 后续如果要区分低频触发帧、定位帧、爆胎帧，
 * 再单独写 Byte9~Byte10 的组包函数。
 */
#define TPMS_APP_FUNC_REUSE_DEFAULT_0        (0xFFU)
#define TPMS_APP_FUNC_REUSE_DEFAULT_1        (0xFFU)


/*
 * ============================================================
 * FFB2 上报结构体
 * ============================================================
 */
typedef struct
{
    /*
     * Byte2 ~ Byte5
     *
     * 注意：
     * 这里直接放 4 个原始 ID 字节，不做大小端转换。
     * 从传感器广播里解析出什么顺序，就按协议要求放什么顺序。
     */
    uint8_t sensor_id[4];

    /*
     * Byte6
     *
     * App 换算：
     * 实际胎压 kPa = pressure_raw * 1.373
     */
    uint8_t pressure_raw;

    /*
     * Byte7
     *
     * App 换算：
     * 实际胎温 ℃ = temperature_raw - 50
     */
    uint8_t temperature_raw;

    /*
     * Byte8
     */
    uint8_t info;

    /*
     * Byte9 ~ Byte10
     */
    uint8_t function_reuse[2];

    /*
     * Byte11
     */
    uint8_t position;

} TpmsAppFfb2Report_t;


/*
 * 组 Byte8 Info 字段
 */
uint8_t TpmsProtocol_makeInfo(uint8_t vendor,
                              uint8_t mode,
                              uint8_t battery_low,
                              uint8_t lf_triggered);

/*
 * 胎压 kPa 转协议原始值。
 *
 * 协议：
 * 胎压 kPa = Pressure * 1.373
 *
 * 所以：
 * Pressure = 胎压 kPa / 1.373
 */
uint8_t TpmsProtocol_pressureKpaToRaw(uint16_t pressure_kpa);

/*
 * 胎温 ℃ 转协议原始值。
 *
 * 协议：
 * 胎温 ℃ = Temperature - 50
 *
 * 所以：
 * Temperature = 胎温 ℃ + 50
 */
uint8_t TpmsProtocol_temperatureCToRaw(int16_t temperature_c);

/*
 * 构造正式 FFB2 12 字节报文。
 */
bool TpmsProtocol_buildFfb2Report(const TpmsAppFfb2Report_t *report,
                                  uint8_t *out_buf,
                                  uint16_t out_buf_len);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_PROTOCOL_H_ */
