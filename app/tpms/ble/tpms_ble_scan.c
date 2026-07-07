/*
 * tpms_ble_scan.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */


#include "tpms_ble_scan.h"

#include <stddef.h>
#include <string.h>

#include "../tpms_app.h"

void TpmsBleScan_init(void)
{
    /*
     * 当前不直接启动 BLE Scan。
     *
     * BLE Scan 的启动仍然放在 app_observer.c / TI BLE_basic 原流程中。
     * 本模块只负责把扫描到的广播数据转换成 TPMS 内部格式。
     */
}

bool TpmsBleScan_handleAdvReport(const uint8_t *addr,
                                 uint8_t addr_type,
                                 const uint8_t *adv_data,
                                 uint16_t adv_len,
                                 int8_t rssi,
                                 uint32_t timestamp_ms)
{
    TpmsBleAdvReport_t report;

    if ((addr == NULL) || (adv_data == NULL))
    {
        return false;
    }

    if (adv_len == 0U)
    {
        return false;
    }

    memset(&report, 0, sizeof(TpmsBleAdvReport_t));

    memcpy(report.addr, addr, sizeof(report.addr));

    report.addr_type = addr_type;
    report.data = adv_data;
    report.len = adv_len;
    report.rssi = rssi;
    report.timestamp_ms = timestamp_ms;

    TPMS_onBleAdvReport(&report);

    return true;
}

