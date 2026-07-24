/*
 * tpms_ble_service.c
 *
 * TPMS BLE service layer
 *
 * FFB1: App -> BLEM
 * FFB2: BLEM -> App / nRF Connect
 */

#include "tpms_ble_service.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../tpms_types.h"
#include "../phone/tpms_protocol.h"

/*
 * simple_gatt_profile.h 需要能找到：
 *
 * SIMPLEGATTPROFILE_CHAR2
 * SIMPLEGATTPROFILE_CHAR2_LEN / SIMPLEGATTPROFILE_CHAR2_MAX_LEN
 * SimpleGattProfile_setParameter()
 * SUCCESS
 *
 * 如果你工程里 simple_gatt_profile.h 的 include 路径不同，
 * 只改下面这一行 include。
 */
#include <ti/ble/profiles/simple_gatt/simple_gatt_profile.h>


#ifndef TPMS_BLE_FFB2_TX_CACHE_LEN
#define TPMS_BLE_FFB2_TX_CACHE_LEN             (32U)
#endif

#ifndef TPMS_BLE_FFB2_TX_QUEUE_SIZE
#define TPMS_BLE_FFB2_TX_QUEUE_SIZE            (8U)
#endif

#ifndef TPMS_BLE_FFB2_RETRY_DEFAULT_COUNT
#define TPMS_BLE_FFB2_RETRY_DEFAULT_COUNT      (3U)
#endif

#ifndef TPMS_BLE_FFB2_RETRY_INTERVAL_MS
#define TPMS_BLE_FFB2_RETRY_INTERVAL_MS        (150U)
#endif


/*
 * ============================================================
 * 调试变量
 * ============================================================
 */

volatile uint32_t g_tpms_ble_ffb2_notify_req_count = 0U;
volatile uint32_t g_tpms_ble_ffb2_notify_ok_count = 0U;
volatile uint32_t g_tpms_ble_ffb2_notify_fail_count = 0U;

volatile uint8_t  g_tpms_ble_ffb2_notify_last_status = 0U;
volatile uint8_t  g_tpms_ble_ffb2_notify_last_len = 0U;
volatile uint8_t  g_tpms_ble_ffb2_notify_last_data[TPMS_BLE_FFB2_TX_CACHE_LEN] = {0};

volatile uint8_t  g_tpms_ble_ffb2_retry_active = 0U;
volatile uint8_t  g_tpms_ble_ffb2_retry_left = 0U;

volatile uint32_t g_tpms_ble_ffb2_queue_push_count = 0U;
volatile uint32_t g_tpms_ble_ffb2_queue_pop_count = 0U;
volatile uint32_t g_tpms_ble_ffb2_queue_drop_count = 0U;
volatile uint8_t  g_tpms_ble_ffb2_queue_count = 0U;


/*
 * ============================================================
 * 内部发送队列
 * ============================================================
 */

typedef struct
{
    uint8_t  used;
    uint8_t  data[TPMS_BLE_FFB2_TX_CACHE_LEN];
    uint16_t len;
    uint8_t  retry_left;
    uint32_t next_tx_ms;
} TpmsBleFfb2TxItem_t;


static TpmsBleFfb2TxItem_t s_tpms_ble_ffb2_tx_queue[TPMS_BLE_FFB2_TX_QUEUE_SIZE];

static uint8_t s_tpms_ble_ffb2_tx_head = 0U;
static uint8_t s_tpms_ble_ffb2_tx_tail = 0U;
static uint8_t s_tpms_ble_ffb2_tx_count = 0U;


/*
 * ============================================================
 * 内部辅助函数
 * ============================================================
 */

static bool TpmsBleService_timeReached(uint32_t now_ms, uint32_t target_ms)
{
    if ((int32_t)(now_ms - target_ms) >= 0)
    {
        return true;
    }

    return false;
}


static void TpmsBleService_updateQueueDebug(void)
{
    g_tpms_ble_ffb2_queue_count = s_tpms_ble_ffb2_tx_count;

    if (s_tpms_ble_ffb2_tx_count == 0U)
    {
        g_tpms_ble_ffb2_retry_active = 0U;
        g_tpms_ble_ffb2_retry_left = 0U;
        return;
    }

    g_tpms_ble_ffb2_retry_active = 1U;
    g_tpms_ble_ffb2_retry_left =
        s_tpms_ble_ffb2_tx_queue[s_tpms_ble_ffb2_tx_head].retry_left;
}


static bool TpmsBleService_queueIsFull(void)
{
    if (s_tpms_ble_ffb2_tx_count >= TPMS_BLE_FFB2_TX_QUEUE_SIZE)
    {
        return true;
    }

    return false;
}


static bool TpmsBleService_queueIsEmpty(void)
{
    if (s_tpms_ble_ffb2_tx_count == 0U)
    {
        return true;
    }

    return false;
}


static void TpmsBleService_popQueue(void)
{
    if (TpmsBleService_queueIsEmpty() == true)
    {
        TpmsBleService_updateQueueDebug();
        return;
    }

    memset(&s_tpms_ble_ffb2_tx_queue[s_tpms_ble_ffb2_tx_head],
           0,
           sizeof(s_tpms_ble_ffb2_tx_queue[s_tpms_ble_ffb2_tx_head]));

    s_tpms_ble_ffb2_tx_head++;

    if (s_tpms_ble_ffb2_tx_head >= TPMS_BLE_FFB2_TX_QUEUE_SIZE)
    {
        s_tpms_ble_ffb2_tx_head = 0U;
    }

    if (s_tpms_ble_ffb2_tx_count > 0U)
    {
        s_tpms_ble_ffb2_tx_count--;
    }

    g_tpms_ble_ffb2_queue_pop_count++;

    TpmsBleService_updateQueueDebug();
}


static bool TpmsBleService_pushQueue(const uint8_t *data,
                                     uint16_t len,
                                     uint8_t repeat_count)
{
    TpmsBleFfb2TxItem_t *item;
    uint16_t copy_len;

    if ((data == NULL) || (len == 0U))
    {
        return false;
    }

    if (TpmsBleService_queueIsFull() == true)
    {
        g_tpms_ble_ffb2_queue_drop_count++;
        TpmsBleService_updateQueueDebug();
        return false;
    }

    copy_len = len;

    if (copy_len > TPMS_BLE_FFB2_TX_CACHE_LEN)
    {
        copy_len = TPMS_BLE_FFB2_TX_CACHE_LEN;
    }

    if (repeat_count == 0U)
    {
        repeat_count = TPMS_BLE_FFB2_RETRY_DEFAULT_COUNT;
    }

    item = &s_tpms_ble_ffb2_tx_queue[s_tpms_ble_ffb2_tx_tail];

    memset(item, 0, sizeof(*item));

    memcpy(item->data,
           data,
           copy_len);

    item->used = 1U;
    item->len = copy_len;
    item->retry_left = repeat_count;

    /*
     * 0 表示下一次 periodic 立即允许发送。
     */
    item->next_tx_ms = 0U;

    s_tpms_ble_ffb2_tx_tail++;

    if (s_tpms_ble_ffb2_tx_tail >= TPMS_BLE_FFB2_TX_QUEUE_SIZE)
    {
        s_tpms_ble_ffb2_tx_tail = 0U;
    }

    s_tpms_ble_ffb2_tx_count++;

    g_tpms_ble_ffb2_queue_push_count++;

    TpmsBleService_updateQueueDebug();

    return true;
}


static bool TpmsBleService_notifyFfb2Once(const uint8_t *data, uint16_t len)
{
    uint8_t status;
    uint8_t copy_len;

    g_tpms_ble_ffb2_notify_req_count++;

    if ((data == NULL) || (len == 0U))
    {
        g_tpms_ble_ffb2_notify_fail_count++;
        g_tpms_ble_ffb2_notify_last_status = 0xFEU;
        g_tpms_ble_ffb2_notify_last_len = 0U;
        return false;
    }

    copy_len = (uint8_t)len;

    if (copy_len > TPMS_BLE_FFB2_TX_CACHE_LEN)
    {
        copy_len = TPMS_BLE_FFB2_TX_CACHE_LEN;
    }

    memset((void *)g_tpms_ble_ffb2_notify_last_data,
           0,
           sizeof(g_tpms_ble_ffb2_notify_last_data));

    memcpy((void *)g_tpms_ble_ffb2_notify_last_data,
           data,
           copy_len);

    g_tpms_ble_ffb2_notify_last_len = copy_len;

    /*
     * SIMPLEGATTPROFILE_CHAR2 对应 FFB2。
     *
     * 如果 App / nRF 已经打开 Notify，
     * SimpleGattProfile_setParameter() 会更新特征值并触发 Notify。
     */
    status = SimpleGattProfile_setParameter(SIMPLEGATTPROFILE_CHAR2,
                                            (uint8_t)len,
                                            (void *)data);

    g_tpms_ble_ffb2_notify_last_status = status;

    if (status == SUCCESS)
    {
        g_tpms_ble_ffb2_notify_ok_count++;
        return true;
    }

    g_tpms_ble_ffb2_notify_fail_count++;
    return false;
}


static uint8_t TpmsBleService_positionFromWheelPos(TpmsWheelPos_t wheel_pos)
{
    uint8_t position;

    switch (wheel_pos)
    {
        case TPMS_WHEEL_LF:
            position = TPMS_APP_POS_LF;
            break;

        case TPMS_WHEEL_RF:
            position = TPMS_APP_POS_RF;
            break;

        case TPMS_WHEEL_RR:
            position = TPMS_APP_POS_RR;
            break;

        case TPMS_WHEEL_LR:
            position = TPMS_APP_POS_LR;
            break;

        default:
            position = TPMS_APP_POS_NONE;
            break;
    }

    return position;
}


/*
 * ============================================================
 * 对外接口
 * ============================================================
 */

void TpmsBleService_init(void)
{
    memset(s_tpms_ble_ffb2_tx_queue,
           0,
           sizeof(s_tpms_ble_ffb2_tx_queue));

    s_tpms_ble_ffb2_tx_head = 0U;
    s_tpms_ble_ffb2_tx_tail = 0U;
    s_tpms_ble_ffb2_tx_count = 0U;

    g_tpms_ble_ffb2_notify_req_count = 0U;
    g_tpms_ble_ffb2_notify_ok_count = 0U;
    g_tpms_ble_ffb2_notify_fail_count = 0U;
    g_tpms_ble_ffb2_notify_last_status = 0U;
    g_tpms_ble_ffb2_notify_last_len = 0U;

    memset((void *)g_tpms_ble_ffb2_notify_last_data,
           0,
           sizeof(g_tpms_ble_ffb2_notify_last_data));

    g_tpms_ble_ffb2_retry_active = 0U;
    g_tpms_ble_ffb2_retry_left = 0U;

    g_tpms_ble_ffb2_queue_push_count = 0U;
    g_tpms_ble_ffb2_queue_pop_count = 0U;
    g_tpms_ble_ffb2_queue_drop_count = 0U;
    g_tpms_ble_ffb2_queue_count = 0U;
}


bool TpmsBleService_notifyFfb2(const uint8_t *data, uint16_t len)
{
    /*
     * 立即发送一次。
     *
     * 兼容旧代码。
     * 如果想提高可靠性，应优先使用 TpmsBleService_queueFfb2()。
     */
    return TpmsBleService_notifyFfb2Once(data, len);
}


bool TpmsBleService_queueFfb2(const uint8_t *data,
                              uint16_t len,
                              uint8_t repeat_count)
{
    bool ret;

    /*
     * 先立即发送一次。
     *
     * 这样即使 TPMS_periodic() 暂时没有被主循环调用，
     * App / nRF 也能马上收到第一包 FFB2。
     */
    (void)TpmsBleService_notifyFfb2Once(data, len);

    /*
     * 再放入队列，后续如果 TPMS_periodic() 被调用，
     * 还可以继续重发。
     */
    ret = TpmsBleService_pushQueue(data, len, repeat_count);

    return ret;
}

bool TpmsBleService_notifyFfb2AppReport(const TpmsAppFfb2Report_t *report)
{
    uint8_t data[TPMS_APP_FFB2_REPORT_LEN];

    if (report == NULL)
    {
        return false;
    }

    memset(data, 0, sizeof(data));

    if (TpmsProtocol_buildFfb2Report(report,
                                     data,
                                     sizeof(data)) == false)
    {
        return false;
    }

    return TpmsBleService_notifyFfb2(data, TPMS_APP_FFB2_REPORT_LEN);
}


bool TpmsBleService_queueFfb2AppReport(const TpmsAppFfb2Report_t *report,
                                       uint8_t repeat_count)
{
    uint8_t data[TPMS_APP_FFB2_REPORT_LEN];

    if (report == NULL)
    {
        return false;
    }

    memset(data, 0, sizeof(data));

    if (TpmsProtocol_buildFfb2Report(report,
                                     data,
                                     sizeof(data)) == false)
    {
        return false;
    }

    return TpmsBleService_queueFfb2(data,
                                    TPMS_APP_FFB2_REPORT_LEN,
                                    repeat_count);
}


bool TpmsBleService_notifyFfb2SensorRaw(const uint8_t sensor_id[4],
                                        uint8_t pressure_raw,
                                        uint8_t temperature_raw,
                                        uint8_t info,
                                        uint8_t function_reuse_0,
                                        uint8_t function_reuse_1,
                                        uint8_t position)
{
    TpmsAppFfb2Report_t report;

    if (sensor_id == NULL)
    {
        return false;
    }

    memset(&report, 0, sizeof(report));

    report.sensor_id[0] = sensor_id[0];
    report.sensor_id[1] = sensor_id[1];
    report.sensor_id[2] = sensor_id[2];
    report.sensor_id[3] = sensor_id[3];

    report.pressure_raw = pressure_raw;
    report.temperature_raw = temperature_raw;
    report.info = info;

    report.function_reuse[0] = function_reuse_0;
    report.function_reuse[1] = function_reuse_1;

    report.position = position;

    return TpmsBleService_notifyFfb2AppReport(&report);
}


bool TpmsBleService_queueFfb2SensorRaw(const uint8_t sensor_id[4],
                                       uint8_t pressure_raw,
                                       uint8_t temperature_raw,
                                       uint8_t info,
                                       uint8_t function_reuse_0,
                                       uint8_t function_reuse_1,
                                       uint8_t position,
                                       uint8_t repeat_count)
{
    TpmsAppFfb2Report_t report;

    if (sensor_id == NULL)
    {
        return false;
    }

    memset(&report, 0, sizeof(report));

    report.sensor_id[0] = sensor_id[0];
    report.sensor_id[1] = sensor_id[1];
    report.sensor_id[2] = sensor_id[2];
    report.sensor_id[3] = sensor_id[3];

    report.pressure_raw = pressure_raw;
    report.temperature_raw = temperature_raw;
    report.info = info;

    report.function_reuse[0] = function_reuse_0;
    report.function_reuse[1] = function_reuse_1;

    report.position = position;

    return TpmsBleService_queueFfb2AppReport(&report, repeat_count);
}


bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data)
{
    uint8_t sensor_id[4];
    uint8_t pressure_raw;
    uint8_t temperature_raw;
    uint8_t info;
    uint8_t position;

    if (wheel_data == NULL)
    {
        return false;
    }

    if (wheel_data->valid == false)
    {
        return false;
    }

    /*
     * sensor_id 当前按 32bit 大端拆成 4 字节。
     *
     * 如果你后面真实传感器 ID 已经有 4 字节数组，
     * 可以再改这里。
     */
    sensor_id[0] = (uint8_t)((wheel_data->sensor_id >> 24) & 0xFFU);
    sensor_id[1] = (uint8_t)((wheel_data->sensor_id >> 16) & 0xFFU);
    sensor_id[2] = (uint8_t)((wheel_data->sensor_id >> 8) & 0xFFU);
    sensor_id[3] = (uint8_t)(wheel_data->sensor_id & 0xFFU);

    /*
     * 优先使用 raw 值。
     * 如果 raw 为 0，可以用工程里的转换函数从 kPa / ℃ 转。
     */
    pressure_raw = wheel_data->pressure_raw;
    temperature_raw = wheel_data->temperature_raw;

    if ((pressure_raw == 0U) && (wheel_data->pressure_kpa != 0U))
    {
        pressure_raw =
            TpmsProtocol_pressureKpaToRaw(wheel_data->pressure_kpa);
    }

    if ((temperature_raw == 0U) && (wheel_data->temperature_c != 0))
    {
        temperature_raw =
            TpmsProtocol_temperatureCToRaw(wheel_data->temperature_c);
    }

    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_RUNNING,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_NOT_TRIGGERED);

    position = TpmsBleService_positionFromWheelPos(wheel_data->wheel_pos);

    return TpmsBleService_notifyFfb2SensorRaw(sensor_id,
                                             pressure_raw,
                                             temperature_raw,
                                             info,
                                             wheel_data->function_reuse[0],
                                             wheel_data->function_reuse[1],
                                             position);
}


void TpmsBleService_periodic(uint32_t now_ms)
{
    TpmsBleFfb2TxItem_t *item;
    bool ok;

    if (TpmsBleService_queueIsEmpty() == true)
    {
        TpmsBleService_updateQueueDebug();
        return;
    }

    item = &s_tpms_ble_ffb2_tx_queue[s_tpms_ble_ffb2_tx_head];

    if (item->used == 0U)
    {
        TpmsBleService_popQueue();
        return;
    }

    if (item->retry_left == 0U)
    {
        TpmsBleService_popQueue();
        return;
    }

    if (item->next_tx_ms != 0U)
    {
        if (TpmsBleService_timeReached(now_ms, item->next_tx_ms) == false)
        {
            TpmsBleService_updateQueueDebug();
            return;
        }
    }

    ok = TpmsBleService_notifyFfb2Once(item->data, item->len);

    /*
     * 不管 SUCCESS 还是 FAIL，都消耗一次发送机会。
     *
     * 如果 BLE 栈忙或者 App 没开 Notify，后面还有 retry。
     */
    if (item->retry_left > 0U)
    {
        item->retry_left--;
    }

    if (item->retry_left == 0U)
    {
        TpmsBleService_popQueue();
    }
    else
    {
        item->next_tx_ms = now_ms + TPMS_BLE_FFB2_RETRY_INTERVAL_MS;
    }

    TpmsBleService_updateQueueDebug();

    (void)ok;
}
