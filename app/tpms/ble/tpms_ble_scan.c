/*
 * tpms_ble_scan.c
 *
 * TPMS 传感器 BLE 广播扫描处理
 */

#include "tpms_ble_scan.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../tpms_config.h"

/*
 * 无串口时，用 CCS Expressions 观察这些变量。
 */

/* 最近一包任意 BLE 广播 */
volatile uint32_t g_tpms_scan_adv_count = 0U;
volatile uint32_t g_tpms_scan_tpms_count = 0U;
volatile int8_t   g_tpms_scan_last_rssi = 0;
volatile uint8_t  g_tpms_scan_last_addr[6] = {0};
volatile uint8_t  g_tpms_scan_last_addr_type = 0U;
volatile uint8_t  g_tpms_scan_last_data[64] = {0};
volatile uint8_t  g_tpms_scan_last_len = 0U;
volatile uint8_t  g_tpms_scan_last_is_tpms = 0U;
volatile uint32_t g_tpms_scan_last_timestamp_ms = 0U;

/* 最近一包识别到的 TPMS 传感器广播 */
volatile int8_t   g_tpms_scan_last_tpms_rssi = 0;
volatile uint8_t  g_tpms_scan_last_tpms_addr[6] = {0};
volatile uint8_t  g_tpms_scan_last_tpms_addr_type = 0U;
volatile uint8_t  g_tpms_scan_last_tpms_data[64] = {0};
volatile uint8_t  g_tpms_scan_last_tpms_len = 0U;


volatile int8_t   g_tpms_scan_best_rssi = -128;
volatile uint8_t  g_tpms_scan_best_addr[6] = {0};
volatile uint8_t  g_tpms_scan_best_addr_type = 0U;
volatile uint8_t  g_tpms_scan_best_data[64] = {0};
volatile uint8_t  g_tpms_scan_best_len = 0U;

void TpmsBleScan_init(void)
{
    g_tpms_scan_adv_count = 0U;
    g_tpms_scan_tpms_count = 0U;
    g_tpms_scan_last_rssi = 0;
    g_tpms_scan_last_addr_type = 0U;
    g_tpms_scan_last_len = 0U;
    g_tpms_scan_last_is_tpms = 0U;
    g_tpms_scan_last_timestamp_ms = 0U;

    memset((void *)g_tpms_scan_last_addr, 0, sizeof(g_tpms_scan_last_addr));
    memset((void *)g_tpms_scan_last_data, 0, sizeof(g_tpms_scan_last_data));

    g_tpms_scan_last_tpms_rssi = 0;
    g_tpms_scan_last_tpms_addr_type = 0U;
    g_tpms_scan_last_tpms_len = 0U;

    memset((void *)g_tpms_scan_last_tpms_addr, 0, sizeof(g_tpms_scan_last_tpms_addr));
    memset((void *)g_tpms_scan_last_tpms_data, 0, sizeof(g_tpms_scan_last_tpms_data));
    /*
       * 调试用：清空 RSSI 最强广播缓存。
       */
    g_tpms_scan_best_rssi = -128;
    g_tpms_scan_best_addr_type = 0U;
    g_tpms_scan_best_len = 0U;

    memset((void *)g_tpms_scan_best_addr, 0, sizeof(g_tpms_scan_best_addr));
    memset((void *)g_tpms_scan_best_data, 0, sizeof(g_tpms_scan_best_data));
}

static bool TpmsBleScan_dataContainsBytes(const uint8_t *data,
                                          uint8_t data_len,
                                          const uint8_t *pattern,
                                          uint8_t pattern_len)
{
    uint8_t i;

    if ((data == NULL) || (pattern == NULL))
    {
        return false;
    }

    if ((pattern_len == 0U) || (data_len < pattern_len))
    {
        return false;
    }

    for (i = 0U; i <= (uint8_t)(data_len - pattern_len); i++)
    {
        if (memcmp(&data[i], pattern, pattern_len) == 0)
        {
            return true;
        }
    }

    return false;
}

static bool TpmsBleScan_debugAddrFilter(const uint8_t *addr)
{
#if (TPMS_DEBUG_FILTER_BY_SENSOR_ADDR != 0U)

    /*
     * nRF Connect 显示地址：
     * D0:39:3F:19:83:06
     */
    static const uint8_t debug_addr_normal[6] =
    {
        TPMS_DEBUG_SENSOR_ADDR_0,
        TPMS_DEBUG_SENSOR_ADDR_1,
        TPMS_DEBUG_SENSOR_ADDR_2,
        TPMS_DEBUG_SENSOR_ADDR_3,
        TPMS_DEBUG_SENSOR_ADDR_4,
        TPMS_DEBUG_SENSOR_ADDR_5
    };

    /*
     * TI BLE Stack 内部地址顺序可能和 nRF Connect 显示相反，
     * 所以同时匹配反序。
     */
    static const uint8_t debug_addr_reverse[6] =
    {
        TPMS_DEBUG_SENSOR_ADDR_5,
        TPMS_DEBUG_SENSOR_ADDR_4,
        TPMS_DEBUG_SENSOR_ADDR_3,
        TPMS_DEBUG_SENSOR_ADDR_2,
        TPMS_DEBUG_SENSOR_ADDR_1,
        TPMS_DEBUG_SENSOR_ADDR_0
    };

    if (addr == NULL)
    {
        return false;
    }

    if (memcmp(addr, debug_addr_normal, 6U) == 0)
    {
        return true;
    }

    if (memcmp(addr, debug_addr_reverse, 6U) == 0)
    {
        return true;
    }

#else
    (void)addr;
#endif

    return false;
}

static bool TpmsBleScan_isTpmsSensorAdv(const uint8_t *addr,
                                        const uint8_t *data,
                                        uint8_t data_len)
{
    static const uint8_t snp_name[] = {'S', 'N', 'P'};

    /*
     * 第一优先级：调试阶段按指定 BLE 地址过滤。
     *
     * 这只是调试手段。
     * 后续正式学习流程时，把 TPMS_DEBUG_FILTER_BY_SENSOR_ADDR 改为 0 即可关闭。
     */
    if (TpmsBleScan_debugAddrFilter(addr) == true)
    {
        return true;
    }

    /*
     * 第二优先级：按广播名判断。
     *
     * 当前传感器名称为 SNP756。
     * 如果后续发现名称在 Scan Response 里，需要把 Observer 改成 Active Scan。
     */
    if (TpmsBleScan_dataContainsBytes(data,
                                      data_len,
                                      snp_name,
                                      sizeof(snp_name)) == true)
    {
        return true;
    }

    /*
     * 第三阶段后续扩展：
     * 等拿到传感器广播协议后，在这里解析 Manufacturer Data。
     *
     * 例如：
     * - 传感器 ID
     * - 胎压
     * - 胎温
     * - 电池状态
     * - 故障状态
     */

    return false;
}

bool TpmsBleScan_handleAdvReport(const uint8_t *addr,
                                 uint8_t addr_type,
                                 const uint8_t *data,
                                 uint8_t data_len,
                                 int8_t rssi,
                                 uint32_t timestamp_ms)
{
    uint8_t copy_len;
    bool is_tpms;

    if ((addr == NULL) || (data == NULL) || (data_len == 0U))
    {
        return false;
    }

    /*
     * 统计所有 BLE 广播包。
     */
    g_tpms_scan_adv_count++;

    /*
     * 保存最近一包任意 BLE 广播。
     * 注意：周围蓝牙设备很多时，这组变量会频繁被覆盖。
     */
    g_tpms_scan_last_addr_type = addr_type;
    g_tpms_scan_last_rssi = rssi;
    g_tpms_scan_last_timestamp_ms = timestamp_ms;

    memcpy((void *)g_tpms_scan_last_addr, addr, 6U);

    copy_len = data_len;

    if (copy_len > sizeof(g_tpms_scan_last_data))
    {
        copy_len = sizeof(g_tpms_scan_last_data);
    }

    memset((void *)g_tpms_scan_last_data, 0, sizeof(g_tpms_scan_last_data));
    memcpy((void *)g_tpms_scan_last_data, data, copy_len);
    g_tpms_scan_last_len = copy_len;

    /*
     * 保存当前扫描过程中 RSSI 最强的一包广播。
     *
     * 作用：
     * 周围蓝牙设备很多时，用这个变量判断 BLEM 实际扫到的最强设备是谁。
     * 胎压传感器和板子距离很近时，通常它的 RSSI 会最强。
     */
    if (rssi > g_tpms_scan_best_rssi)
    {
        g_tpms_scan_best_rssi = rssi;
        g_tpms_scan_best_addr_type = addr_type;

        memcpy((void *)g_tpms_scan_best_addr, addr, 6U);

        memset((void *)g_tpms_scan_best_data, 0, sizeof(g_tpms_scan_best_data));
        memcpy((void *)g_tpms_scan_best_data, data, copy_len);

        g_tpms_scan_best_len = copy_len;
    }

    /*
     * 判断是否为 TPMS 传感器广播。
     */
    is_tpms = TpmsBleScan_isTpmsSensorAdv(addr, data, data_len);

    if (is_tpms == true)
    {
        g_tpms_scan_tpms_count++;
        g_tpms_scan_last_is_tpms = 1U;

        /*
         * 专门保存 TPMS 广播。
         * 这组变量不会被其他普通 BLE 广播覆盖。
         */
        g_tpms_scan_last_tpms_addr_type = addr_type;
        g_tpms_scan_last_tpms_rssi = rssi;

        memcpy((void *)g_tpms_scan_last_tpms_addr, addr, 6U);

        memset((void *)g_tpms_scan_last_tpms_data, 0, sizeof(g_tpms_scan_last_tpms_data));
        memcpy((void *)g_tpms_scan_last_tpms_data, data, copy_len);

        g_tpms_scan_last_tpms_len = copy_len;

        return true;
    }

    g_tpms_scan_last_is_tpms = 0U;
    return false;
}
