/*
 * tpms_parser.h
 *
 *  Created on: 2026年6月25日
 *      Author: ASUS
 */

#ifndef APP_PROFILES_TPMS_PARSER_H_
#define APP_PROFILES_TPMS_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t sensorId;
    uint16_t pressureKpa;
    int8_t temperatureC;
    bool batteryLow;        //电池低电压标志.返回true/false
    bool blowoutWarning;   //爆胎告警标志
    uint8_t seq;           //单字节序列号
} TpmsData_t;

bool Tpms_parseTestPacket(const uint8_t *data, uint8_t len, TpmsData_t *out);



#endif /* APP_PROFILES_TPMS_PARSER_H_ */
