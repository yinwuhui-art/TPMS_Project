/*
 * tpms_sensor_parser.h
 *
 * TPMS sensor data parser
 */

#ifndef TPMS_SENSOR_PARSER_H_
#define TPMS_SENSOR_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * SNP756 Indicate 数据长度
 *
 * 目前 nRF Connect 看到：
 * 00 38 0F C7 00 00 00
 */
#define TPMS_SNP756_INDICATE_MIN_LEN         (7U)

/*
 * SNP756 Indicate 字节位置
 *
 * Byte1：温度 raw
 * Byte2：状态 raw
 * Byte3：压力 raw
 */
#define TPMS_SNP756_IDX_TEMPERATURE_RAW      (1U)
#define TPMS_SNP756_IDX_STATUS_RAW           (2U)
#define TPMS_SNP756_IDX_PRESSURE_RAW         (3U)


void TpmsSensorParser_init(void);

bool TpmsSensorParser_parseSnp756Indicate(const uint8_t *data,
                                          uint16_t len,
                                          uint8_t *pressure_raw,
                                          uint8_t *temperature_raw,
                                          uint8_t *status_raw);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_SENSOR_PARSER_H_ */
