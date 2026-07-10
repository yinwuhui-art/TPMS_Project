#include "tpms_ble_scan.h"

#include <stddef.h>
#include <string.h>

#include "ti/ble/app_util/menu/menu_module.h"

#include "../tpms_app.h"

void TpmsBleScan_init(void)
{
}

bool TpmsBleScan_handleAdvReport(const uint8_t *addr,
                                 uint8_t addr_type,
                                 const uint8_t *adv_data,
                                 uint16_t adv_len,
                                 int8_t rssi,
                                 uint32_t timestamp_ms)
{
    TpmsBleAdvReport_t report;
    uint16_t i;

    if ((addr == NULL) || (adv_data == NULL))
    {
        return false;
    }

    if (adv_len == 0U)
    {
        return false;
    }

    MenuModule_printf(0, 0,
                      "TPMS ADV addr=%02X:%02X:%02X:%02X:%02X:%02X type=%d len=%d rssi=%d",
                      addr[5], addr[4], addr[3],
                      addr[2], addr[1], addr[0],
                      addr_type,
                      adv_len,
                      rssi);

    for (i = 0U; i < adv_len; i++)
    {
        MenuModule_printf(0, 0,
                          "ADV[%d]=0x%02X",
                          i,
                          adv_data[i]);
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
