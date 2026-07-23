/*
 * tpms_app.h
 *
 * TPMS application layer
 */

#ifndef TPMS_APP_H_
#define TPMS_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tpms_types.h"


void TPMS_init(void);

void TPMS_start(void);

void TPMS_stop(void);

bool TPMS_isStarted(void);

void TPMS_periodic(uint32_t now_ms);

void TPMS_onPhoneRxData(const uint8_t *data, uint16_t len);


/*
 * 如果 app_observer.c 里还在使用这个接口，
 * 先保留声明，避免类型报错。
 *
 * 后面实际扫描逻辑主要走 TpmsBleScan_handleAdvReport()。
 */
bool TPMS_onBleAdvReport(const TpmsBleAdvReport_t *report);


/*
 * 如果其他模块需要读取轮位数据，也先保留。
 */
const TpmsWheelData_t *TPMS_getWheelData(TpmsWheelPos_t wheel_pos);


#ifdef __cplusplus
}
#endif

#endif /* TPMS_APP_H_ */
