/*
 * tpms_ble_service.h
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

void TpmsBleService_periodic(uint32_t now_ms);

void TpmsBleService_init(void);

/*
 * 直接通过 FFB2 发送原始数据。
 */
bool TpmsBleService_notifyFfb2(const uint8_t *data, uint16_t len);

/*
 * 通过正式 12 字节协议结构体发送 FFB2。
 */
bool TpmsBleService_notifyFfb2AppReport(const TpmsAppFfb2Report_t *report);

/*
 * 通过参数快速发送一帧正式 FFB2 12 字节报文。
 */
bool TpmsBleService_notifyFfb2SensorRaw(const uint8_t sensor_id[4],
                                        uint8_t pressure_raw,
                                        uint8_t temperature_raw,
                                        uint8_t info,
                                        uint8_t function_reuse_0,
                                        uint8_t function_reuse_1,
                                        uint8_t position);

/*
 * 兼容旧接口。
 *
 * 如果 tpms_app.c 里还调用这个函数，可以先保留。
 * 后续真正解析 TpmsWheelData_t 后，再把里面的数据映射到正式协议。
 */
bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_BLE_SERVICE_H_ */
