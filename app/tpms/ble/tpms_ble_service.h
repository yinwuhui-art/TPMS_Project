/*
 * tpms_ble_service.h
 *
 * TPMS BLE service layer
 *
 * FFB1: App -> BLEM
 * FFB2: BLEM -> App / nRF Connect
 */

#ifndef TPMS_BLE_SERVICE_H_
#define TPMS_BLE_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"
#include "../phone/tpms_protocol.h"


/*
 * ============================================================
 * FFB2 Notify 发送缓存 / 重发参数
 * ============================================================
 */

/*
 * FFB2 当前是 12 字节协议：
 *
 * Byte0~Byte1 固定 00 01
 * Byte2~Byte5 Sensor ID
 * Byte6 PressureRaw
 * Byte7 TemperatureRaw
 * Byte8 Info
 * Byte9~Byte10 FunctionReuse
 * Byte11 Position / 学习标志位
 *
 * 这里缓存长度给 32 字节，后续协议扩展也够用。
 */
#define TPMS_BLE_FFB2_TX_CACHE_LEN             (32U)

/*
 * FFB2 发送队列长度。
 *
 * 现在四轮学习最多会连续入队 4 包：
 * 0x01 / 0x03 / 0x0B / 0x0F
 *
 * 设置 8 比较安全。
 */
#define TPMS_BLE_FFB2_TX_QUEUE_SIZE            (8U)

/*
 * 默认重复发送次数。
 *
 * 每包重复发送 3 次，提高 nRF / App 收包概率。
 */
#define TPMS_BLE_FFB2_RETRY_DEFAULT_COUNT      (3U)

/*
 * 重发间隔，单位 ms。
 *
 * 150ms 比较稳，不会太密集抢占 BLE 连接事件。
 */
#define TPMS_BLE_FFB2_RETRY_INTERVAL_MS        (150U)


/*
 * ============================================================
 * FFB2 Notify 调试变量
 * ============================================================
 */

/*
 * FFB2 Notify 请求次数。
 *
 * 只要调用到底层 SimpleGattProfile_setParameter()，
 * 这个计数就会增加。
 */
extern volatile uint32_t g_tpms_ble_ffb2_notify_req_count;

/*
 * FFB2 Notify 成功次数。
 *
 * SimpleGattProfile_setParameter() 返回 SUCCESS 时增加。
 */
extern volatile uint32_t g_tpms_ble_ffb2_notify_ok_count;

/*
 * FFB2 Notify 失败次数。
 *
 * SimpleGattProfile_setParameter() 返回非 SUCCESS 时增加。
 */
extern volatile uint32_t g_tpms_ble_ffb2_notify_fail_count;

/*
 * 最近一次 FFB2 Notify 返回状态。
 *
 * 重点看这个：
 * SUCCESS 表示 BLEM 侧调用发送成功。
 */
extern volatile uint8_t  g_tpms_ble_ffb2_notify_last_status;

/*
 * 最近一次 FFB2 Notify 长度。
 *
 * 正常应该是 12。
 */
extern volatile uint8_t  g_tpms_ble_ffb2_notify_last_len;

/*
 * 最近一次 FFB2 Notify 数据。
 *
 * 重点看：
 *
 * last_data[0]  = 0x00
 * last_data[1]  = 0x01
 * last_data[11] = position
 *
 * 学习过程期望：
 * 0x01 / 0x03 / 0x0B / 0x0F
 */
extern volatile uint8_t  g_tpms_ble_ffb2_notify_last_data[TPMS_BLE_FFB2_TX_CACHE_LEN];


/*
 * ============================================================
 * FFB2 重发队列调试变量
 * ============================================================
 */

/*
 * 当前是否还有 FFB2 重发任务。
 *
 * 0 = 无任务
 * 1 = 有任务
 */
extern volatile uint8_t  g_tpms_ble_ffb2_retry_active;

/*
 * 当前队首报文剩余重发次数。
 */
extern volatile uint8_t  g_tpms_ble_ffb2_retry_left;

/*
 * 入队次数。
 *
 * 每次调用 TpmsBleService_queueFfb2() 成功后增加。
 */
extern volatile uint32_t g_tpms_ble_ffb2_queue_push_count;

/*
 * 出队次数。
 *
 * 某个报文重发完成后增加。
 */
extern volatile uint32_t g_tpms_ble_ffb2_queue_pop_count;

/*
 * 队列满导致丢弃次数。
 *
 * 正常应该一直是 0。
 */
extern volatile uint32_t g_tpms_ble_ffb2_queue_drop_count;

/*
 * 当前队列里还有几包待发送。
 */
extern volatile uint8_t  g_tpms_ble_ffb2_queue_count;


/*
 * ============================================================
 * 对外接口
 * ============================================================
 */

/*
 * 初始化 TPMS BLE Service。
 *
 * 清空 FFB2 发送队列和调试计数。
 */
void TpmsBleService_init(void);


/*
 * 立即发送一包 FFB2 原始数据。
 *
 * 兼容旧代码。
 * 当前建议学习成功上报优先使用 TpmsBleService_queueFfb2SensorRaw()。
 */
bool TpmsBleService_notifyFfb2(const uint8_t *data,
                               uint16_t len);


/*
 * 将一包 FFB2 原始数据放入发送队列。
 *
 * data:
 *   FFB2 原始数据。
 *
 * len:
 *   数据长度，当前正式协议为 12 字节。
 *
 * repeat_count:
 *   重复发送次数。
 *   如果传 0，会使用 TPMS_BLE_FFB2_RETRY_DEFAULT_COUNT。
 */
bool TpmsBleService_queueFfb2(const uint8_t *data,
                              uint16_t len,
                              uint8_t repeat_count);


/*
 * 立即发送一个 FFB2 App Report。
 *
 * report 会通过 TpmsProtocol_buildFfb2Report() 打包成 12 字节。
 */
bool TpmsBleService_notifyFfb2AppReport(const TpmsAppFfb2Report_t *report);


/*
 * 将一个 FFB2 App Report 放入发送队列。
 */
bool TpmsBleService_queueFfb2AppReport(const TpmsAppFfb2Report_t *report,
                                       uint8_t repeat_count);


/*
 * 立即发送一包 FFB2 Sensor Raw 数据。
 *
 * 当前 FFB2 12 字节协议：
 *
 * Byte0~Byte1 = 00 01
 * Byte2~Byte5 = sensor_id[0..3]
 * Byte6 = pressure_raw
 * Byte7 = temperature_raw
 * Byte8 = info
 * Byte9 = function_reuse_0
 * Byte10 = function_reuse_1
 * Byte11 = position
 */
bool TpmsBleService_notifyFfb2SensorRaw(const uint8_t sensor_id[4],
                                        uint8_t pressure_raw,
                                        uint8_t temperature_raw,
                                        uint8_t info,
                                        uint8_t function_reuse_0,
                                        uint8_t function_reuse_1,
                                        uint8_t position);


/*
 * 将一包 FFB2 Sensor Raw 数据放入发送队列。
 *
 * 推荐用于学习成功上报：
 *
 * LF 后：position = 0x01
 * RF 后：position = 0x03
 * RR 后：position = 0x0B
 * LR 后：position = 0x0F
 *
 * repeat_count 建议传 3。
 */
bool TpmsBleService_queueFfb2SensorRaw(const uint8_t sensor_id[4],
                                       uint8_t pressure_raw,
                                       uint8_t temperature_raw,
                                       uint8_t info,
                                       uint8_t function_reuse_0,
                                       uint8_t function_reuse_1,
                                       uint8_t position,
                                       uint8_t repeat_count);


/*
 * 根据轮胎数据发送 FFB2。
 *
 * 后续真实胎压胎温接入后使用。
 */
bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data);


/*
 * TPMS BLE Service 周期函数。
 *
 * 必须在 TPMS_periodic() 里面周期调用：
 *
 * TpmsBleService_periodic(now_ms);
 *
 * 它负责处理 FFB2 队列和重发。
 */
void TpmsBleService_periodic(uint32_t now_ms);


#ifdef __cplusplus
}
#endif

#endif /* TPMS_BLE_SERVICE_H_ */
