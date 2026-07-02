/*
 * tpms_protocol.c
 *
 *  Created on: 2026Äê7ÔÂ1ÈÕ
 *      Author: ASUS
 */


#include "tpms_protocol.h"

extern void tpms_notify_to_app(uint8_t *data, uint16_t len);

void tpms_app_rx_handler(uint8_t *data, uint16_t len)
{
    if (data == 0 || len == 0)
    {
        return;
    }

    switch (data[0])
    {
        case TPMS_CMD_WAKEUP:
            tpms_send_mock_tire_data(POS_LF);
            break;

        case TPMS_CMD_START_LEARN:
            tpms_send_mock_tire_data(POS_LF);
            break;

        default:
            break;
    }
}

void tpms_send_mock_tire_data(uint8_t position)
{
    tpms_notify_frame_t frame = {0};

    frame.fixed0 = 0x00;
    frame.fixed1 = 0x01;

    frame.sensor_id[0] = 0x12;
    frame.sensor_id[1] = 0x34;
    frame.sensor_id[2] = 0x56;
    frame.sensor_id[3] = 0x78;

    frame.pressure = 200;       // 200 * 1.373 = 274.6 kPa
    frame.temperature = 75;     // 75 - 50 = 25 ¡æ

    frame.info = 0x24;
    frame.function_reuse = 0x00;
    frame.position = position;  // Byte10

    tpms_notify_to_app((uint8_t *)&frame, TPMS_NOTIFY_FRAME_LEN);
}

