/*
 * tpms_sensor_parser.c
 *
 * TPMS sensor data parser
 */

#include "tpms_sensor_parser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


void TpmsSensorParser_init(void)
{
    /*
     * 当前解析模块暂时没有内部状态。
     */
}


bool TpmsSensorParser_parseSnp756Indicate(const uint8_t *data,
                                          uint16_t len,
                                          uint8_t *pressure_raw,
                                          uint8_t *temperature_raw,
                                          uint8_t *status_raw)
{
    if ((data == NULL) ||
        (pressure_raw == NULL) ||
        (temperature_raw == NULL) ||
        (status_raw == NULL))
    {
        return false;
    }

    if (len < TPMS_SNP756_INDICATE_MIN_LEN)
    {
        return false;
    }

    *temperature_raw = data[TPMS_SNP756_IDX_TEMPERATURE_RAW];
    *status_raw = data[TPMS_SNP756_IDX_STATUS_RAW];
    *pressure_raw = data[TPMS_SNP756_IDX_PRESSURE_RAW];

    return true;
}
