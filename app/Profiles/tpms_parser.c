/*
 * tpms_parser.c
 *
 *  Created on: 2026年6月25日
 *      Author: ASUS
 */

#include "tpms_parser.h"

static uint8_t Tpms_xorChecksum(const uint8_t *data, uint8_t len)
{
    uint8_t cs = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        cs ^= data[i];
    }

    return cs;
}

bool Tpms_parseTestPacket(const uint8_t *data, uint8_t len, TpmsData_t *out)
{
    if ((data == 0) || (out == 0))
    {
        return false;
    }

    if (len < 10)
    {
        return false;
    }

    if (data[0] != 0xA5)
    {
        return false;
    }

    uint8_t calcCs = Tpms_xorChecksum(data, 9);

    if (calcCs != data[9])
    {
        return false;
    }

    out->sensorId =
        ((uint32_t)data[1] << 24) |
        ((uint32_t)data[2] << 16) |
        ((uint32_t)data[3] << 8)  |
        ((uint32_t)data[4]);

    uint8_t pressureRaw = data[5];
    uint8_t tempRaw = data[6];
    uint8_t status = data[7];

    /*
     * pressureKpa = round(raw * 1.3725)
     * 用整数近似，避免 float：
     * raw * 1.3725 = raw * 13725 / 10000
     */
    out->pressureKpa = (uint16_t)((pressureRaw * 13725 + 5000) / 10000);

    /*
     * temperatureC = raw - 50
     */
    out->temperatureC = (int8_t)tempRaw - 50;

    /*
     * 这里先用测试位定义：
     * bit2 = battery low
     * bit3 = blowout warning
     * 后面拿到真实 TPMS 协议后再替换
     */
    out->batteryLow = (status & 0x04) ? true : false;
    out->blowoutWarning = (status & 0x08) ? true : false;

    out->seq = data[8];

    return true;
}



