/*
 * tpms_storage.c
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */
#include "tpms_storage.h"

void TpmsStorage_init(void)
{
    /*
     * 后面这里对接 TI NVS / Flash，用于保存四轮传感器 ID。
     */
}

bool TpmsStorage_loadSensorIds(void)
{
    return false;
}

bool TpmsStorage_saveSensorIds(void)
{
    return false;
}



