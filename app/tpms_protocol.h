/*
 * tpms_protocol.h
 *
 *  Created on: 2026年7月1日
 *      Author: ASUS
 */

#ifndef APP_TPMS_PROTOCOL_H_
#define APP_TPMS_PROTOCOL_H_

#include <stdint.h>

#define TPMS_NOTIFY_FRAME_LEN     11

#define TPMS_CMD_WAKEUP           0x01
#define TPMS_CMD_START_LEARN      0x02

#define POS_LF                    0x01    // 左前
#define POS_RF                    0x02    // 右前
#define POS_LR                    0x04    // 左后
#define POS_RR                    0x08    // 右后

typedef struct
{
    uint8_t fixed0;          // Byte0 = 0x00
    uint8_t fixed1;          // Byte1 = 0x01
    uint8_t sensor_id[4];    // Byte2-Byte5
    uint8_t pressure;        // Byte6
    uint8_t temperature;     // Byte7
    uint8_t info;            // Byte8
    uint8_t function_reuse;  // Byte9
    uint8_t position;        // Byte10
} tpms_notify_frame_t;

void tpms_app_rx_handler(uint8_t *data, uint16_t len);
void tpms_send_mock_tire_data(uint8_t position);




#endif /* APP_TPMS_PROTOCOL_H_ */
