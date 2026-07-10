/*
 * tpms_ble_service.h
 *
 * TPMS BLE 上报接口
 */

#ifndef TPMS_BLE_SERVICE_H_
#define TPMS_BLE_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../tpms_types.h"

/*
 * 初始化 TPMS BLE Service。
 */
void TpmsBleService_init(void);

/*
 * 通过 FFB2 Notify / Read 特征值向手机 App 上报原始数据。
 *
 * FFB2 UUID:
 * 0000FFB2-0000-1000-8000-00805F9B34FB
 */
bool TpmsBleService_notifyFfb2(const uint8_t *data, uint16_t len);

/*
 * 兼容旧框架接口：
 * tpms_app.c 里原来调用的是这个函数。
 *
 * 作用：
 * 把单个轮胎数据打包后，通过 FFB2 Notify 发给手机 App。
 */
bool TpmsBleService_notifyWheelData(const TpmsWheelData_t *wheel_data);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_BLE_SERVICE_H_ */
