/*
 * tpms_storage.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_STORAGE_H_
#define TPMS_STORAGE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void TpmsStorage_init(void);

bool TpmsStorage_loadSensorIds(void);

bool TpmsStorage_saveSensorIds(void);

#ifdef __cplusplus
}
#endif

#endif
