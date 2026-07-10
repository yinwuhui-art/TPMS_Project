/*
 * tpms_ble_uuid.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_BLE_UUID_H_
#define TPMS_BLE_UUID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TPMS BLE GATT Characteristic UUID
 *
 * App -> BLEM/CC2745:
 * 0000FFB1-0000-1000-8000-00805F9B34FB
 * 属性: Write / Write Without Response
 * 用途: 手机 App 下发唤醒、学习、刷新等命令
 */
#define TPMS_CHAR_UUID_APP_WRITE        (0xFFB1U)

/*
 * BLEM/CC2745 -> App:
 * 0000FFB2-0000-1000-8000-00805F9B34FB
 * 属性: Notify / Read
 * 用途: BLEM 上报胎压、胎温、传感器 ID、状态、故障、学习结果等数据
 */
#define TPMS_CHAR_UUID_NOTIFY_READ      (0xFFB2U)

/*
 * functionID definitions for FFB1 Write
 *
 * App 向 FFB1 写 1 个字节:
 * 0x01: 唤醒胎压传感器
 * 0x02: 开始手动学习 Start_Learn_TPMS
 */
#define TPMS_FUNCTION_ID_WAKEUP_SENSOR      (0x01U)
#define TPMS_FUNCTION_ID_START_LEARN_TPMS   (0x02U)

#ifdef __cplusplus
}
#endif

#endif /* TPMS_BLE_UUID_H_ */
