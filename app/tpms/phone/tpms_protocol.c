/*
 * tpms_protocol.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */



#include "tpms_protocol.h"

#include <stddef.h>
#include <string.h>

static uint8_t TpmsProtocol_checksum(const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint8_t sum = 0U;

    if (data == NULL)
    {
        return 0U;
    }

    for (i = 0U; i < len; i++)
    {
        sum = (uint8_t)(sum + data[i]);
    }

    return sum;
}

static void TpmsProtocol_putU16Le(uint8_t *buf, uint16_t value)
{
    buf[0] = (uint8_t)(value & 0xFFU);
    buf[1] = (uint8_t)((value >> 8U) & 0xFFU);
}

static void TpmsProtocol_putS16Le(uint8_t *buf, int16_t value)
{
    TpmsProtocol_putU16Le(buf, (uint16_t)value);
}

static void TpmsProtocol_putU32Le(uint8_t *buf, uint32_t value)
{
    buf[0] = (uint8_t)(value & 0xFFU);
    buf[1] = (uint8_t)((value >> 8U) & 0xFFU);
    buf[2] = (uint8_t)((value >> 16U) & 0xFFU);
    buf[3] = (uint8_t)((value >> 24U) & 0xFFU);
}

bool TpmsProtocol_decode(const uint8_t *data,
                         uint16_t len,
                         TpmsProtocolFrame_t *out_frame)
{
    uint8_t payload_len;
    uint16_t total_len;
    uint8_t checksum_calc;
    uint8_t checksum_recv;

    if ((data == NULL) || (out_frame == NULL))
    {
        return false;
    }

    if (len < 5U)
    {
        return false;
    }

    if ((data[0] != TPMS_PROTOCOL_HEAD0) ||
        (data[1] != TPMS_PROTOCOL_HEAD1))
    {
        return false;
    }

    payload_len = data[3];

    if (payload_len > TPMS_PROTOCOL_PAYLOAD_MAX_LEN)
    {
        return false;
    }

    total_len = (uint16_t)(5U + payload_len);

    if (len < total_len)
    {
        return false;
    }

    checksum_recv = data[4U + payload_len];
    checksum_calc = TpmsProtocol_checksum(&data[2], (uint16_t)(2U + payload_len));

    if (checksum_calc != checksum_recv)
    {
        return false;
    }

    memset(out_frame, 0, sizeof(TpmsProtocolFrame_t));

    out_frame->cmd = data[2];
    out_frame->payload_len = payload_len;

    if (payload_len > 0U)
    {
        memcpy(out_frame->payload, &data[4], payload_len);
    }

    return true;
}

bool TpmsProtocol_encode(uint8_t cmd,
                         const uint8_t *payload,
                         uint16_t payload_len,
                         uint8_t *out_data,
                         uint16_t out_max_len,
                         uint16_t *out_len)
{
    uint16_t total_len;

    if ((out_data == NULL) || (out_len == NULL))
    {
        return false;
    }

    if (payload_len > TPMS_PROTOCOL_PAYLOAD_MAX_LEN)
    {
        return false;
    }

    if ((payload_len > 0U) && (payload == NULL))
    {
        return false;
    }

    total_len = (uint16_t)(5U + payload_len);

    if (out_max_len < total_len)
    {
        return false;
    }

    out_data[0] = TPMS_PROTOCOL_HEAD0;
    out_data[1] = TPMS_PROTOCOL_HEAD1;
    out_data[2] = cmd;
    out_data[3] = (uint8_t)payload_len;

    if (payload_len > 0U)
    {
        memcpy(&out_data[4], payload, payload_len);
    }

    out_data[4U + payload_len] =
        TpmsProtocol_checksum(&out_data[2], (uint16_t)(2U + payload_len));

    *out_len = total_len;

    return true;
}

bool TpmsProtocol_packOneWheelData(TpmsWheelPos_t wheel_pos,
                                   const TpmsWheelData_t *wheel_data,
                                   uint8_t *out_payload,
                                   uint16_t out_max_len,
                                   uint16_t *out_payload_len)
{
    uint16_t index = 0U;

    /*
     * 单轮 payload 格式：
     *
     * Byte0      wheel_pos
     * Byte1      valid
     * Byte2~5    sensor_id, little endian
     * Byte6~7    pressure_kpa, int16, little endian
     * Byte8~9    temperature_c, int16, little endian
     * Byte10~11  battery_mv, uint16, little endian
     * Byte12~13  status, uint16, little endian
     * Byte14     rssi
     * Byte15~18  last_rx_ms, uint32, little endian
     *
     * 总长度：19 bytes
     */

    if ((wheel_data == NULL) ||
        (out_payload == NULL) ||
        (out_payload_len == NULL))
    {
        return false;
    }

    if (out_max_len < 19U)
    {
        return false;
    }

    out_payload[index++] = (uint8_t)wheel_pos;
    out_payload[index++] = (wheel_data->valid == true) ? 1U : 0U;

    TpmsProtocol_putU32Le(&out_payload[index], wheel_data->sensor_id);
    index += 4U;

    TpmsProtocol_putS16Le(&out_payload[index], wheel_data->pressure_kpa);
    index += 2U;

    TpmsProtocol_putS16Le(&out_payload[index], wheel_data->temperature_c);
    index += 2U;

    TpmsProtocol_putU16Le(&out_payload[index], wheel_data->battery_mv);
    index += 2U;

    TpmsProtocol_putU16Le(&out_payload[index], wheel_data->status);
    index += 2U;

    out_payload[index++] = (uint8_t)wheel_data->rssi;

    TpmsProtocol_putU32Le(&out_payload[index], wheel_data->last_rx_ms);
    index += 4U;

    *out_payload_len = index;

    return true;
}
