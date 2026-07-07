/*
 * tpms_ble_wakeup.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#include "tpms_ble_wakeup.h"

static TpmsBleWakeupState_t s_wakeup_state = TPMS_BLE_WAKEUP_STATE_IDLE;
static uint32_t s_wakeup_request_time_ms = 0UL;

void TpmsBleWakeup_init(void)
{
    s_wakeup_state = TPMS_BLE_WAKEUP_STATE_IDLE;
    s_wakeup_request_time_ms = 0UL;
}

bool TpmsBleWakeup_request(void)
{
    if ((s_wakeup_state == TPMS_BLE_WAKEUP_STATE_REQUESTED) ||
        (s_wakeup_state == TPMS_BLE_WAKEUP_STATE_SENDING))
    {
        return true;
    }

    s_wakeup_state = TPMS_BLE_WAKEUP_STATE_REQUESTED;

    return true;
}

void TpmsBleWakeup_periodic(uint32_t now_ms)
{
    switch (s_wakeup_state)
    {
        case TPMS_BLE_WAKEUP_STATE_IDLE:
            break;

        case TPMS_BLE_WAKEUP_STATE_REQUESTED:
            /*
             * 后面这里真正发送 TPMS 传感器蓝牙唤醒指令。
             *
             * 可能的实现方式：
             * 1. 发送特定 BLE 广播包
             * 2. 连接传感器后写入唤醒命令
             * 3. 发送 Manufacturer Specific Data
             *
             * 当前先做状态机占位，保证架构正确。
             */
            s_wakeup_state = TPMS_BLE_WAKEUP_STATE_SENDING;
            s_wakeup_request_time_ms = now_ms;
            break;

        case TPMS_BLE_WAKEUP_STATE_SENDING:
            /*
             * 当前没有接真实 BLE API，先模拟发送完成。
             * 后面接入 TI BLE API 后，在发送完成回调里置 DONE。
             */
            if ((now_ms - s_wakeup_request_time_ms) > 10UL)
            {
                s_wakeup_state = TPMS_BLE_WAKEUP_STATE_DONE;
            }
            break;

        case TPMS_BLE_WAKEUP_STATE_DONE:
            s_wakeup_state = TPMS_BLE_WAKEUP_STATE_IDLE;
            break;

        case TPMS_BLE_WAKEUP_STATE_ERROR:
        default:
            s_wakeup_state = TPMS_BLE_WAKEUP_STATE_IDLE;
            break;
    }
}

TpmsBleWakeupState_t TpmsBleWakeup_getState(void)
{
    return s_wakeup_state;
}

bool TpmsBleWakeup_isPending(void)
{
    if ((s_wakeup_state == TPMS_BLE_WAKEUP_STATE_REQUESTED) ||
        (s_wakeup_state == TPMS_BLE_WAKEUP_STATE_SENDING))
    {
        return true;
    }

    return false;
}


