/*
 * tpms_protocol.c
 *
 * BLEM -> Phone APP FFB2 protocol
 */

#include "tpms_protocol.h"

#include <string.h>


uint8_t TpmsProtocol_makeInfo(uint8_t vendor,
                              uint8_t mode,
                              uint8_t battery_low,
                              uint8_t lf_triggered)
{
    uint8_t info = 0U;

    /*
     * Byte8:
     *
     * bit7~bit5：厂家
     * bit4~bit2：模式
     * bit1     ：电池状态
     * bit0     ：低频触发状态
     */
    info |= (uint8_t)((vendor & 0x07U) << 5);
    info |= (uint8_t)((mode & 0x07U) << 2);
    info |= (uint8_t)((battery_low & 0x01U) << 1);
    info |= (uint8_t)(lf_triggered & 0x01U);

    return info;
}


uint8_t TpmsProtocol_pressureKpaToRaw(uint16_t pressure_kpa)
{
    uint32_t raw;

    /*
     * 协议：
     * 胎压 kPa = Pressure * 1.373
     *
     * Pressure = 胎压 kPa / 1.373
     *          = 胎压 kPa * 1000 / 1373
     *
     * +686 用于四舍五入。
     */
    raw = (((uint32_t)pressure_kpa * 1000UL) + 686UL) / 1373UL;

    if (raw > 255UL)
    {
        raw = 255UL;
    }

    return (uint8_t)raw;
}


uint8_t TpmsProtocol_temperatureCToRaw(int16_t temperature_c)
{
    int16_t raw;

    /*
     * 协议：
     * 胎温 ℃ = Temperature - 50
     *
     * Temperature = 胎温 ℃ + 50
     */
    raw = (int16_t)(temperature_c + 50);

    if (raw < 0)
    {
        raw = 0;
    }

    if (raw > 255)
    {
        raw = 255;
    }

    return (uint8_t)raw;
}


bool TpmsProtocol_buildFfb2Report(const TpmsAppFfb2Report_t *report,
                                  uint8_t *out_buf,
                                  uint16_t out_buf_len)
{
    if ((report == NULL) || (out_buf == NULL))
    {
        return false;
    }

    if (out_buf_len < TPMS_APP_FFB2_REPORT_LEN)
    {
        return false;
    }

    /*
     * Byte0 ~ Byte1：固定值 0x00 0x01
     */
    out_buf[TPMS_APP_FFB2_IDX_FIXED_0] = TPMS_APP_FFB2_FIXED_BYTE0;
    out_buf[TPMS_APP_FFB2_IDX_FIXED_1] = TPMS_APP_FFB2_FIXED_BYTE1;

    /*
     * Byte2 ~ Byte5：传感器 ID
     */
    out_buf[TPMS_APP_FFB2_IDX_SENSOR_ID_0] = report->sensor_id[0];
    out_buf[TPMS_APP_FFB2_IDX_SENSOR_ID_1] = report->sensor_id[1];
    out_buf[TPMS_APP_FFB2_IDX_SENSOR_ID_2] = report->sensor_id[2];
    out_buf[TPMS_APP_FFB2_IDX_SENSOR_ID_3] = report->sensor_id[3];

    /*
     * Byte6：Pressure
     */
    out_buf[TPMS_APP_FFB2_IDX_PRESSURE] = report->pressure_raw;

    /*
     * Byte7：Temperature
     */
    out_buf[TPMS_APP_FFB2_IDX_TEMPERATURE] = report->temperature_raw;

    /*
     * Byte8：Info
     */
    out_buf[TPMS_APP_FFB2_IDX_INFO] = report->info;

    /*
     * Byte9 ~ Byte10：FunctionReuse
     */
    out_buf[TPMS_APP_FFB2_IDX_FUNC_REUSE_0] = report->function_reuse[0];
    out_buf[TPMS_APP_FFB2_IDX_FUNC_REUSE_1] = report->function_reuse[1];

    /*
     * Byte11：position
     */
    out_buf[TPMS_APP_FFB2_IDX_POSITION] = report->position;

    return true;
}
