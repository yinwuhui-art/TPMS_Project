/*
 * tpms_ble_scan.c
 *
 * TPMS 传感器 BLE 广播扫描处理
 *
 * 当前版本：
 *
 * 1. 地址白名单作为主过滤条件。
 * 2. SNP756 名称作为验证条件。
 * 3. 同时兼容完整 MAC 正序 / 反序。
 * 4. 同时兼容 MAC 后三字节正序 / 反序。
 *
 * 原因：
 *
 * TPMS 的名称 SNP756 不一定每一包广播都带。
 * 有时名称在 Scan Response 中，而 Manufacturer Data 在 Advertising Data 中。
 * CC2745 不一定像手机 nRF Connect 一样自动合并显示。
 */

#include "tpms_ble_scan.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../tpms_config.h"
#include "../core/tpms_learning.h"


/*
 * ============================================================
 * 无串口调试变量
 * ============================================================
 */

volatile uint32_t g_tpms_scan_adv_count = 0U;
volatile uint32_t g_tpms_scan_tpms_count = 0U;

volatile int8_t   g_tpms_scan_last_rssi = 0;
volatile uint8_t  g_tpms_scan_last_addr[6] = {0};
volatile uint8_t  g_tpms_scan_last_addr_type = 0U;
volatile uint8_t  g_tpms_scan_last_data[64] = {0};
volatile uint8_t  g_tpms_scan_last_len = 0U;
volatile uint8_t  g_tpms_scan_last_is_tpms = 0U;
volatile uint32_t g_tpms_scan_last_timestamp_ms = 0U;

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

volatile uint32_t g_tpms_scan_name_match_count = 0U;

volatile uint32_t g_tpms_scan_addr_whitelist_match_count = 0U;
volatile uint32_t g_tpms_scan_name_and_addr_match_count = 0U;

volatile uint8_t  g_tpms_scan_last_whitelist_index = 0xFFU;
volatile uint8_t  g_tpms_scan_last_name_verified = 0U;
volatile uint8_t  g_tpms_scan_last_addr_match_mode = TPMS_SCAN_ADDR_MATCH_NONE;

volatile uint32_t g_tpms_scan_name_only_count = 0U;
volatile uint8_t  g_tpms_scan_name_only_last_addr[6] = {0};

volatile uint32_t g_tpms_scan_white_addr_hit_count[4] = {0U, 0U, 0U, 0U};
volatile uint8_t  g_tpms_scan_white_name_verified[4] = {0U, 0U, 0U, 0U};
volatile uint8_t  g_tpms_scan_white_last_addr[4][6] = {{0}};

volatile uint8_t g_tpms_scan_last_match_reason = TPMS_SCAN_MATCH_NONE;

volatile uint8_t g_tpms_scan_last_mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN] = {0};
volatile uint8_t g_tpms_scan_last_mfr_len = 0U;

volatile uint32_t g_tpms_capture_total_count = 0U;
volatile uint8_t  g_tpms_capture_write_index = 0U;

volatile uint8_t  g_tpms_capture_match_reason[TPMS_SCAN_CAPTURE_LOG_SIZE] = {0};
volatile int8_t   g_tpms_capture_rssi[TPMS_SCAN_CAPTURE_LOG_SIZE] = {0};
volatile uint8_t  g_tpms_capture_addr_type[TPMS_SCAN_CAPTURE_LOG_SIZE] = {0};
volatile uint8_t  g_tpms_capture_addr[TPMS_SCAN_CAPTURE_LOG_SIZE][6] = {{0}};

volatile uint8_t  g_tpms_capture_len[TPMS_SCAN_CAPTURE_LOG_SIZE] = {0};
volatile uint8_t  g_tpms_capture_data[TPMS_SCAN_CAPTURE_LOG_SIZE][TPMS_SCAN_CAPTURE_DATA_MAX_LEN] = {{0}};

volatile uint8_t  g_tpms_capture_mfr_len[TPMS_SCAN_CAPTURE_LOG_SIZE] = {0};
volatile uint8_t  g_tpms_capture_mfr_data[TPMS_SCAN_CAPTURE_LOG_SIZE][TPMS_SCAN_CAPTURE_MFR_MAX_LEN] = {{0}};

volatile uint8_t  g_tpms_capture_max_mfr_len = 0U;
volatile uint8_t  g_tpms_capture_max_mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN] = {0};
volatile uint8_t  g_tpms_capture_max_mfr_addr[6] = {0};
volatile int8_t   g_tpms_capture_max_mfr_rssi = 0;

static uint8_t s_tpms_scan_current_match_reason = TPMS_SCAN_MATCH_NONE;


/*
 * ============================================================
 * 初始化
 * ============================================================
 */

void TpmsBleScan_init(void)
{
    g_tpms_scan_adv_count = 0U;
    g_tpms_scan_tpms_count = 0U;

    g_tpms_scan_last_rssi = 0;
    g_tpms_scan_last_addr_type = 0U;
    g_tpms_scan_last_len = 0U;
    g_tpms_scan_last_is_tpms = 0U;
    g_tpms_scan_last_timestamp_ms = 0U;

    memset((void *)g_tpms_scan_last_addr,
           0,
           sizeof(g_tpms_scan_last_addr));

    memset((void *)g_tpms_scan_last_data,
           0,
           sizeof(g_tpms_scan_last_data));

    g_tpms_scan_last_tpms_rssi = 0;
    g_tpms_scan_last_tpms_addr_type = 0U;
    g_tpms_scan_last_tpms_len = 0U;

    memset((void *)g_tpms_scan_last_tpms_addr,
           0,
           sizeof(g_tpms_scan_last_tpms_addr));

    memset((void *)g_tpms_scan_last_tpms_data,
           0,
           sizeof(g_tpms_scan_last_tpms_data));

    g_tpms_scan_best_rssi = -128;
    g_tpms_scan_best_addr_type = 0U;
    g_tpms_scan_best_len = 0U;

    memset((void *)g_tpms_scan_best_addr,
           0,
           sizeof(g_tpms_scan_best_addr));

    memset((void *)g_tpms_scan_best_data,
           0,
           sizeof(g_tpms_scan_best_data));

    g_tpms_scan_name_match_count = 0U;

    g_tpms_scan_addr_whitelist_match_count = 0U;
    g_tpms_scan_name_and_addr_match_count = 0U;

    g_tpms_scan_last_whitelist_index = 0xFFU;
    g_tpms_scan_last_name_verified = 0U;
    g_tpms_scan_last_addr_match_mode = TPMS_SCAN_ADDR_MATCH_NONE;

    g_tpms_scan_name_only_count = 0U;

    memset((void *)g_tpms_scan_name_only_last_addr,
           0,
           sizeof(g_tpms_scan_name_only_last_addr));

    memset((void *)g_tpms_scan_white_addr_hit_count,
           0,
           sizeof(g_tpms_scan_white_addr_hit_count));

    memset((void *)g_tpms_scan_white_name_verified,
           0,
           sizeof(g_tpms_scan_white_name_verified));

    memset((void *)g_tpms_scan_white_last_addr,
           0,
           sizeof(g_tpms_scan_white_last_addr));

    g_tpms_scan_last_match_reason = TPMS_SCAN_MATCH_NONE;

    g_tpms_scan_last_mfr_len = 0U;

    memset((void *)g_tpms_scan_last_mfr_data,
           0,
           sizeof(g_tpms_scan_last_mfr_data));

    g_tpms_capture_total_count = 0U;
    g_tpms_capture_write_index = 0U;

    memset((void *)g_tpms_capture_match_reason,
           0,
           sizeof(g_tpms_capture_match_reason));

    memset((void *)g_tpms_capture_rssi,
           0,
           sizeof(g_tpms_capture_rssi));

    memset((void *)g_tpms_capture_addr_type,
           0,
           sizeof(g_tpms_capture_addr_type));

    memset((void *)g_tpms_capture_addr,
           0,
           sizeof(g_tpms_capture_addr));

    memset((void *)g_tpms_capture_len,
           0,
           sizeof(g_tpms_capture_len));

    memset((void *)g_tpms_capture_data,
           0,
           sizeof(g_tpms_capture_data));

    memset((void *)g_tpms_capture_mfr_len,
           0,
           sizeof(g_tpms_capture_mfr_len));

    memset((void *)g_tpms_capture_mfr_data,
           0,
           sizeof(g_tpms_capture_mfr_data));

    g_tpms_capture_max_mfr_len = 0U;
    g_tpms_capture_max_mfr_rssi = 0;

    memset((void *)g_tpms_capture_max_mfr_data,
           0,
           sizeof(g_tpms_capture_max_mfr_data));

    memset((void *)g_tpms_capture_max_mfr_addr,
           0,
           sizeof(g_tpms_capture_max_mfr_addr));

    s_tpms_scan_current_match_reason = TPMS_SCAN_MATCH_NONE;
}


/*
 * ============================================================
 * Local Name 解析
 * ============================================================
 */

static bool TpmsBleScan_nameFieldMatches(const uint8_t *data,
                                         uint8_t data_len,
                                         const uint8_t *target_name,
                                         uint8_t target_name_len)
{
    uint8_t index = 0U;

    if ((data == NULL) || (target_name == NULL))
    {
        return false;
    }

    if ((data_len == 0U) || (target_name_len == 0U))
    {
        return false;
    }

    while (index < data_len)
    {
        uint8_t field_len;
        uint8_t ad_type;
        uint8_t name_len;
        uint16_t last_index;

        field_len = data[index];

        if (field_len == 0U)
        {
            break;
        }

        /*
         * field_len 不包含 Length 字节本身，
         * 但包含 AD Type + AD Data。
         *
         * 当前字段最后一个字节下标：
         * index + field_len
         */
        last_index = (uint16_t)index + (uint16_t)field_len;

        if (last_index >= data_len)
        {
            break;
        }

        ad_type = data[index + 1U];

        if ((ad_type == 0x08U) || (ad_type == 0x09U))
        {
            name_len = (uint8_t)(field_len - 1U);

            if (name_len == target_name_len)
            {
                if (memcmp(&data[index + 2U],
                           target_name,
                           target_name_len) == 0)
                {
                    return true;
                }
            }
        }

        index = (uint8_t)(index + field_len + 1U);
    }

    return false;
}


static bool TpmsBleScan_deviceNameFilter(const uint8_t *data,
                                         uint8_t data_len)
{
#if (TPMS_FILTER_BY_DEVICE_NAME != 0U)

    static const uint8_t target_name[TPMS_TARGET_NAME_LEN] =
    {
        TPMS_TARGET_NAME_BYTE_0,
        TPMS_TARGET_NAME_BYTE_1,
        TPMS_TARGET_NAME_BYTE_2,
        TPMS_TARGET_NAME_BYTE_3,
        TPMS_TARGET_NAME_BYTE_4,
        TPMS_TARGET_NAME_BYTE_5
    };

    if ((data == NULL) || (data_len == 0U))
    {
        return false;
    }

    return TpmsBleScan_nameFieldMatches(data,
                                        data_len,
                                        target_name,
                                        TPMS_TARGET_NAME_LEN);
#else
    (void)data;
    (void)data_len;
    return false;
#endif
}


/*
 * ============================================================
 * 地址白名单过滤
 * ============================================================
 */

static bool TpmsBleScan_addrEqualsNormal(const uint8_t *addr,
                                         const uint8_t ref_addr[6])
{
    if ((addr == NULL) || (ref_addr == NULL))
    {
        return false;
    }

    return ((addr[0] == ref_addr[0]) &&
            (addr[1] == ref_addr[1]) &&
            (addr[2] == ref_addr[2]) &&
            (addr[3] == ref_addr[3]) &&
            (addr[4] == ref_addr[4]) &&
            (addr[5] == ref_addr[5]));
}


static bool TpmsBleScan_addrEqualsReverse(const uint8_t *addr,
                                          const uint8_t ref_addr[6])
{
    if ((addr == NULL) || (ref_addr == NULL))
    {
        return false;
    }

    return ((addr[0] == ref_addr[5]) &&
            (addr[1] == ref_addr[4]) &&
            (addr[2] == ref_addr[3]) &&
            (addr[3] == ref_addr[2]) &&
            (addr[4] == ref_addr[1]) &&
            (addr[5] == ref_addr[0]));
}


/*
 * 后三字节正序匹配。
 *
 * nRF:
 * D0:39:3F:19:83:06
 *
 * 后三字节：
 * 19:83:06
 */
static bool TpmsBleScan_addrSuffixEqualsNormal(const uint8_t *addr,
                                               const uint8_t ref_addr[6])
{
    if ((addr == NULL) || (ref_addr == NULL))
    {
        return false;
    }

    return ((addr[3] == ref_addr[3]) &&
            (addr[4] == ref_addr[4]) &&
            (addr[5] == ref_addr[5]));
}


/*
 * 后三字节反序匹配。
 *
 * nRF:
 * D0:39:3F:19:83:06
 *
 * TI 回调中可能是：
 * 06 83 19 3F 39 D0
 *
 * nRF 后三字节 19:83:06
 * 对应 TI addr[2], addr[1], addr[0]
 */
static bool TpmsBleScan_addrSuffixEqualsReverse(const uint8_t *addr,
                                                const uint8_t ref_addr[6])
{
    if ((addr == NULL) || (ref_addr == NULL))
    {
        return false;
    }

    return ((addr[2] == ref_addr[3]) &&
            (addr[1] == ref_addr[4]) &&
            (addr[0] == ref_addr[5]));
}


static bool TpmsBleScan_addrIsZero(const uint8_t ref_addr[6])
{
    if (ref_addr == NULL)
    {
        return true;
    }

    return ((ref_addr[0] == 0U) &&
            (ref_addr[1] == 0U) &&
            (ref_addr[2] == 0U) &&
            (ref_addr[3] == 0U) &&
            (ref_addr[4] == 0U) &&
            (ref_addr[5] == 0U));
}


static bool TpmsBleScan_addrInWhitelist(const uint8_t *addr,
                                        uint8_t *matched_index)
{
#if (TPMS_FILTER_BY_ADDR_WHITELIST != 0U)

    uint8_t i;

    static const uint8_t white_addr[TPMS_SENSOR_ADDR_NUM][6] =
    {
        {
            TPMS_SENSOR_LF_ADDR_0,
            TPMS_SENSOR_LF_ADDR_1,
            TPMS_SENSOR_LF_ADDR_2,
            TPMS_SENSOR_LF_ADDR_3,
            TPMS_SENSOR_LF_ADDR_4,
            TPMS_SENSOR_LF_ADDR_5
        },
        {
            TPMS_SENSOR_RF_ADDR_0,
            TPMS_SENSOR_RF_ADDR_1,
            TPMS_SENSOR_RF_ADDR_2,
            TPMS_SENSOR_RF_ADDR_3,
            TPMS_SENSOR_RF_ADDR_4,
            TPMS_SENSOR_RF_ADDR_5
        },
        {
            TPMS_SENSOR_RR_ADDR_0,
            TPMS_SENSOR_RR_ADDR_1,
            TPMS_SENSOR_RR_ADDR_2,
            TPMS_SENSOR_RR_ADDR_3,
            TPMS_SENSOR_RR_ADDR_4,
            TPMS_SENSOR_RR_ADDR_5
        },
        {
            TPMS_SENSOR_LR_ADDR_0,
            TPMS_SENSOR_LR_ADDR_1,
            TPMS_SENSOR_LR_ADDR_2,
            TPMS_SENSOR_LR_ADDR_3,
            TPMS_SENSOR_LR_ADDR_4,
            TPMS_SENSOR_LR_ADDR_5
        }
    };

    if (matched_index != NULL)
    {
        *matched_index = 0xFFU;
    }

    g_tpms_scan_last_addr_match_mode = TPMS_SCAN_ADDR_MATCH_NONE;

    if (addr == NULL)
    {
        return false;
    }

    for (i = 0U; i < TPMS_SENSOR_ADDR_NUM; i++)
    {
        if (TpmsBleScan_addrIsZero(white_addr[i]) == true)
        {
            continue;
        }

        /*
         * 第一优先级：完整地址正序匹配。
         */
        if (TpmsBleScan_addrEqualsNormal(addr, white_addr[i]) == true)
        {
            if (matched_index != NULL)
            {
                *matched_index = i;
            }

            g_tpms_scan_last_addr_match_mode =
                TPMS_SCAN_ADDR_MATCH_FULL_NORMAL;

            return true;
        }

        /*
         * 第二优先级：完整地址反序匹配。
         */
        if (TpmsBleScan_addrEqualsReverse(addr, white_addr[i]) == true)
        {
            if (matched_index != NULL)
            {
                *matched_index = i;
            }

            g_tpms_scan_last_addr_match_mode =
                TPMS_SCAN_ADDR_MATCH_FULL_REVERSE;

            return true;
        }

        /*
         * 第三优先级：后三字节正序匹配。
         *
         * 这一步是为了避免前 3 字节、地址类型、
         * 地址顺序处理导致漏匹配。
         */
        if (TpmsBleScan_addrSuffixEqualsNormal(addr, white_addr[i]) == true)
        {
            if (matched_index != NULL)
            {
                *matched_index = i;
            }

            g_tpms_scan_last_addr_match_mode =
                TPMS_SCAN_ADDR_MATCH_SUFFIX_NORMAL;

            return true;
        }

        /*
         * 第四优先级：后三字节反序匹配。
         */
        if (TpmsBleScan_addrSuffixEqualsReverse(addr, white_addr[i]) == true)
        {
            if (matched_index != NULL)
            {
                *matched_index = i;
            }

            g_tpms_scan_last_addr_match_mode =
                TPMS_SCAN_ADDR_MATCH_SUFFIX_REVERSE;

            return true;
        }
    }

#else
    (void)addr;
    (void)matched_index;
#endif

    return false;
}


/*
 * ============================================================
 * Manufacturer Data 提取
 * ============================================================
 */

static void TpmsBleScan_extractManufacturerData(const uint8_t *data,
                                                uint8_t data_len,
                                                uint8_t *mfr_data,
                                                uint8_t *mfr_len)
{
    uint8_t index = 0U;

    if (mfr_len != NULL)
    {
        *mfr_len = 0U;
    }

    if ((data == NULL) || (mfr_data == NULL) || (mfr_len == NULL))
    {
        return;
    }

    memset(mfr_data, 0, TPMS_SCAN_CAPTURE_MFR_MAX_LEN);

    while (index < data_len)
    {
        uint8_t field_len;
        uint8_t ad_type;
        uint8_t copy_len;
        uint16_t last_index;

        field_len = data[index];

        if (field_len == 0U)
        {
            break;
        }

        last_index = (uint16_t)index + (uint16_t)field_len;

        if (last_index >= data_len)
        {
            break;
        }

        ad_type = data[index + 1U];

        if (ad_type == 0xFFU)
        {
            copy_len = (uint8_t)(field_len - 1U);

            if (copy_len > TPMS_SCAN_CAPTURE_MFR_MAX_LEN)
            {
                copy_len = TPMS_SCAN_CAPTURE_MFR_MAX_LEN;
            }

            memcpy(mfr_data,
                   &data[index + 2U],
                   copy_len);

            *mfr_len = copy_len;

            return;
        }

        index = (uint8_t)(index + field_len + 1U);
    }
}


static void TpmsBleScan_saveLastManufacturerData(const uint8_t *data,
                                                 uint8_t data_len)
{
    uint8_t mfr_len;
    uint8_t mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN];

    memset(mfr_data, 0, sizeof(mfr_data));
    mfr_len = 0U;

    TpmsBleScan_extractManufacturerData(data,
                                        data_len,
                                        mfr_data,
                                        &mfr_len);

    g_tpms_scan_last_mfr_len = mfr_len;

    memset((void *)g_tpms_scan_last_mfr_data,
           0,
           sizeof(g_tpms_scan_last_mfr_data));

    if (mfr_len > 0U)
    {
        memcpy((void *)g_tpms_scan_last_mfr_data,
               mfr_data,
               mfr_len);
    }
}


/*
 * ============================================================
 * 抓包缓存
 * ============================================================
 */

static void TpmsBleScan_captureTpmsPacket(const uint8_t *addr,
                                          uint8_t addr_type,
                                          const uint8_t *data,
                                          uint8_t data_len,
                                          int8_t rssi,
                                          uint8_t match_reason)
{
    uint8_t index;
    uint8_t copy_len;
    uint8_t mfr_len;
    uint8_t mfr_data[TPMS_SCAN_CAPTURE_MFR_MAX_LEN];

    if ((addr == NULL) || (data == NULL))
    {
        return;
    }

    index = g_tpms_capture_write_index;

    if (data_len > TPMS_SCAN_CAPTURE_DATA_MAX_LEN)
    {
        copy_len = TPMS_SCAN_CAPTURE_DATA_MAX_LEN;
    }
    else
    {
        copy_len = data_len;
    }

    memset((void *)g_tpms_capture_data[index],
           0,
           TPMS_SCAN_CAPTURE_DATA_MAX_LEN);

    memset((void *)g_tpms_capture_mfr_data[index],
           0,
           TPMS_SCAN_CAPTURE_MFR_MAX_LEN);

    g_tpms_capture_match_reason[index] = match_reason;
    g_tpms_capture_rssi[index] = rssi;
    g_tpms_capture_addr_type[index] = addr_type;
    g_tpms_capture_len[index] = copy_len;

    memcpy((void *)g_tpms_capture_addr[index],
           addr,
           6U);

    memcpy((void *)g_tpms_capture_data[index],
           data,
           copy_len);

    memset(mfr_data, 0, sizeof(mfr_data));
    mfr_len = 0U;

    TpmsBleScan_extractManufacturerData(data,
                                        data_len,
                                        mfr_data,
                                        &mfr_len);

    g_tpms_capture_mfr_len[index] = mfr_len;

    if (mfr_len > 0U)
    {
        memcpy((void *)g_tpms_capture_mfr_data[index],
               mfr_data,
               mfr_len);
    }

    /*
     * 记录目前见过的最长 Manufacturer Data。
     */
    if (mfr_len > g_tpms_capture_max_mfr_len)
    {
        g_tpms_capture_max_mfr_len = mfr_len;
        g_tpms_capture_max_mfr_rssi = rssi;

        memcpy((void *)g_tpms_capture_max_mfr_addr,
               addr,
               6U);

        memset((void *)g_tpms_capture_max_mfr_data,
               0,
               TPMS_SCAN_CAPTURE_MFR_MAX_LEN);

        if (mfr_len > 0U)
        {
            memcpy((void *)g_tpms_capture_max_mfr_data,
                   mfr_data,
                   mfr_len);
        }
    }

    g_tpms_capture_total_count++;

    g_tpms_capture_write_index++;

    if (g_tpms_capture_write_index >= TPMS_SCAN_CAPTURE_LOG_SIZE)
    {
        g_tpms_capture_write_index = 0U;
    }
}


/*
 * ============================================================
 * TPMS 广播判断
 * ============================================================
 */

static bool TpmsBleScan_isTpmsSensorAdv(const uint8_t *addr,
                                        const uint8_t *data,
                                        uint8_t data_len)
{
    bool name_match;
    bool addr_match;
    uint8_t matched_index;

    matched_index = 0xFFU;
    name_match = false;
    addr_match = false;

    s_tpms_scan_current_match_reason = TPMS_SCAN_MATCH_NONE;

    /*
     * 名称检测只做验证，不做强制过滤。
     *
     * 原因：
     * 有些包可能没有 Local Name，
     * 但地址已经能证明它是四个目标 TPMS 之一。
     */
    name_match = TpmsBleScan_deviceNameFilter(data, data_len);

    if (name_match == true)
    {
        g_tpms_scan_name_match_count++;
    }

    /*
     * 地址白名单作为主过滤条件。
     */
    addr_match = TpmsBleScan_addrInWhitelist(addr, &matched_index);

    if (addr_match == false)
    {
        /*
         * 如果名称是 SNP756，但地址没有命中白名单，
         * 保存这个地址，方便判断是不是 MAC 配错或顺序不对。
         */
        if (name_match == true)
        {
            g_tpms_scan_name_only_count++;

            if (addr != NULL)
            {
                memcpy((void *)g_tpms_scan_name_only_last_addr,
                       addr,
                       6U);
            }

            s_tpms_scan_current_match_reason = TPMS_SCAN_MATCH_BY_NAME;
        }

        return false;
    }

    /*
     * 地址命中白名单，说明是四个目标 TPMS 之一。
     */
    g_tpms_scan_addr_whitelist_match_count++;
    g_tpms_scan_last_whitelist_index = matched_index;

    if (matched_index < 4U)
    {
        g_tpms_scan_white_addr_hit_count[matched_index]++;

        memcpy((void *)g_tpms_scan_white_last_addr[matched_index],
               addr,
               6U);
    }

    if (name_match == true)
    {
        g_tpms_scan_name_and_addr_match_count++;
        g_tpms_scan_last_name_verified = 1U;

        if (matched_index < 4U)
        {
            g_tpms_scan_white_name_verified[matched_index] = 1U;
        }

        s_tpms_scan_current_match_reason =
            TPMS_SCAN_MATCH_BY_NAME_AND_ADDR;
    }
    else
    {
        g_tpms_scan_last_name_verified = 0U;

        /*
         * 当前包没有 SNP756 名称，但地址已经命中白名单。
         * 仍然认为是目标 TPMS。
         */
        s_tpms_scan_current_match_reason =
            TPMS_SCAN_MATCH_BY_ADDR;
    }

    return true;
}


/*
 * ============================================================
 * 外部入口：处理 BLE 广播
 * ============================================================
 */

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
     */
    g_tpms_scan_last_addr_type = addr_type;
    g_tpms_scan_last_rssi = rssi;
    g_tpms_scan_last_timestamp_ms = timestamp_ms;

    memcpy((void *)g_tpms_scan_last_addr,
           addr,
           6U);

    copy_len = data_len;

    if (copy_len > TPMS_SCAN_CAPTURE_DATA_MAX_LEN)
    {
        copy_len = TPMS_SCAN_CAPTURE_DATA_MAX_LEN;
    }

    memset((void *)g_tpms_scan_last_data,
           0,
           sizeof(g_tpms_scan_last_data));

    memcpy((void *)g_tpms_scan_last_data,
           data,
           copy_len);

    g_tpms_scan_last_len = copy_len;

    /*
     * 保存 RSSI 最强的一包广播。
     */
    if (rssi > g_tpms_scan_best_rssi)
    {
        g_tpms_scan_best_rssi = rssi;
        g_tpms_scan_best_addr_type = addr_type;

        memcpy((void *)g_tpms_scan_best_addr,
               addr,
               6U);

        memset((void *)g_tpms_scan_best_data,
               0,
               sizeof(g_tpms_scan_best_data));

        memcpy((void *)g_tpms_scan_best_data,
               data,
               copy_len);

        g_tpms_scan_best_len = copy_len;
    }

    /*
     * TPMS 过滤：
     *
     * 地址白名单为主，SNP756 名称为验证。
     */
    is_tpms = TpmsBleScan_isTpmsSensorAdv(addr, data, data_len);

    if (is_tpms == true)
    {
        g_tpms_scan_tpms_count++;
        g_tpms_scan_last_is_tpms = 1U;
        g_tpms_scan_last_match_reason = s_tpms_scan_current_match_reason;

        /*
         * 保存最近一包识别到的 TPMS 广播。
         */
        g_tpms_scan_last_tpms_addr_type = addr_type;
        g_tpms_scan_last_tpms_rssi = rssi;

        memcpy((void *)g_tpms_scan_last_tpms_addr,
               addr,
               6U);

        memset((void *)g_tpms_scan_last_tpms_data,
               0,
               sizeof(g_tpms_scan_last_tpms_data));

        memcpy((void *)g_tpms_scan_last_tpms_data,
               data,
               copy_len);

        g_tpms_scan_last_tpms_len = copy_len;

        /*
         * 保存当前包的 Manufacturer Data。
         */
        TpmsBleScan_saveLastManufacturerData(data, data_len);

        /*
         * 保存最近 16 包命中白名单的 TPMS 广播。
         */
        TpmsBleScan_captureTpmsPacket(addr,
                                      addr_type,
                                      data,
                                      data_len,
                                      rssi,
                                      s_tpms_scan_current_match_reason);

        /*
         * 手动自学习入口。
         *
         * 学习模块内部继续判断：
         * FunctionReuse bit1 是否为 1。
         */
        TpmsLearning_onTpmsAdv(
                g_tpms_scan_last_whitelist_index,
                addr,
                addr_type,
                rssi,
                g_tpms_scan_last_mfr_data,
                g_tpms_scan_last_mfr_len);
        return true;
    }

    g_tpms_scan_last_is_tpms = 0U;

    return false;
}
