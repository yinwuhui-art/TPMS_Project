/*
 * tpms_app.c
 *
 * TPMS application layer
 */

#include "tpms_app.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "tpms_config.h"
#include "tpms_types.h"

#include "core/tpms_wheel_manager.h"
#include "core/tpms_fault.h"
#include "core/tpms_localization.h"
#include "core/tpms_learning.h"

#include "sensor/tpms_sensor_parser.h"

#include "ble/tpms_ble_scan.h"
#include "ble/tpms_ble_service.h"
#include "ble/tpms_ble_wakeup.h"

#include "phone/tpms_phone_cmd.h"
#include "phone/tpms_protocol.h"

#include "storage/tpms_storage.h"


/*
 * ============================================================
 * 本文件内部状态变量
 * ============================================================
 */

static bool s_tpms_started = false;


/*
 * ============================================================
 * 内部函数声明
 * ============================================================
 */

static void TpmsApp_sendTestFfb2Report(uint8_t position);


/*
 * ============================================================
 * TPMS 模块初始化
 * ============================================================
 */

void TPMS_init(void)
{
    /*
     * 各业务模块初始化
     */
    TpmsWheelManager_init();
    TpmsFault_init();
    TpmsLocalization_init();
    TpmsSensorParser_init();

    /*
     * BLE 扫描 / GATT 服务初始化
     */
    TpmsBleScan_init();
    TpmsBleService_init();

    /*
     * 存储、手机命令、自学习初始化
     */
    TpmsStorage_init();
    TpmsPhoneCmd_init();
    TpmsLearning_init();

    s_tpms_started = false;
}


/*
 * ============================================================
 * TPMS 模块启动
 * ============================================================
 */

void TPMS_start(void)
{
    s_tpms_started = true;
}


/*
 * ============================================================
 * TPMS 模块停止
 * ============================================================
 */

void TPMS_stop(void)
{
    s_tpms_started = false;

    /*
     * 如果正在学习，停止学习模式。
     */
    TpmsLearning_stopManual();
}


/*
 * ============================================================
 * TPMS 是否已经启动
 * ============================================================
 */

bool TPMS_isStarted(void)
{
    return s_tpms_started;
}


/*
 * ============================================================
 * TPMS 周期任务
 * ============================================================
 *
 * 现在主要保留接口。
 * 后面如果加入超时检测、周期上报、Flash 保存等逻辑，
 * 可以放在这里。
 */

void TPMS_periodic(uint32_t now_ms)
{
    if (s_tpms_started == false)
    {
        return;
    }

    /*
     * 当前 BLE Service 周期函数里暂时可以为空。
     */
    TpmsBleService_periodic(now_ms);
}


/*
 * ============================================================
 * 手机 App 写入 FFB1 后的处理入口
 * ============================================================
 *
 * FFB1:
 * 0000FFB1-0000-1000-8000-00805F9B34FB
 *
 * App -> BLEM
 */

void TPMS_onPhoneRxData(const uint8_t *data, uint16_t len)
{
    TpmsPhoneCmd_t cmd;
    TpmsPhoneCmdResult_t result;

    if ((data == NULL) || (len == 0U))
    {
        return;
    }

    memset(&cmd, 0, sizeof(cmd));

    if (TpmsPhoneCmd_parse(data, len, &cmd) == false)
    {
        return;
    }

    result = TpmsPhoneCmd_handle(&cmd);

    switch (result)
    {
        case TPMS_PHONE_CMD_RESULT_CONNECT_REQUESTED:
        {
            /*
             * App 连接后的握手请求。
             *
             * 之前 App 会写 FFB1 = 00。
             * 为了保持连接流程，仍然返回一帧测试 FFB2。
             *
             * Byte11 position = 0x00，表示不是具体轮位学习结果。
             */
            TpmsApp_sendTestFfb2Report(TPMS_APP_POS_NONE);

            break;
        }

        case TPMS_PHONE_CMD_RESULT_WAKEUP_REQUESTED:
        {
            /*
             * App 写 FFB1 = 01。
             *
             * 当前阶段暂时返回一帧测试 FFB2，
             * 用于证明 App -> BLEM -> App 通路正常。
             *
             * 真正唤醒 TPMS 传感器的逻辑后面再接 LF / Central / 写命令。
             */
            TpmsApp_sendTestFfb2Report(TPMS_APP_POS_NONE);

            break;
        }

        case TPMS_PHONE_CMD_RESULT_START_LEARN:
        {
            /*
             * App 写 FFB1 = 02。
             *
             * 文档里的 Start_Learn_TPMS。
             *
             * 注意：
             * 这里不要直接发 FFB2 学习成功帧。
             *
             * 正确流程：
             *
             * 1. 这里调用 TpmsLearning_startManual()
             * 2. BLEM 进入手动学习模式
             * 3. 学习人员按 LF -> RF -> RR -> LR 顺序低频触发
             * 4. tpms_ble_scan.c 扫到对应白名单地址
             * 5. tpms_ble_scan.c 调用 TpmsLearning_onTpmsAdv()
             * 6. tpms_learning.c 判断 FunctionReuse bit1 = 1
             * 7. 学习成功后，由 tpms_learning.c 通过 FFB2 Notify 通知 App
             */
            TpmsLearning_startManual();

            break;
        }

        case TPMS_PHONE_CMD_RESULT_QUERY_DATA:
        {
            /*
             * App 查询数据。
             *
             * 当前阶段还没有把 BLEM Central 连接 SNP756 的真实胎压胎温
             * 接进来，所以这里先不做真实数据上报。
             *
             * 后续接入 05000200 Indicate 后，可以在这里上报已学习轮位的
             * 最新胎压胎温。
             */
            break;
        }

        case TPMS_PHONE_CMD_RESULT_STOP_LEARN:
        {
            /*
             * 停止手动学习。
             */
            TpmsLearning_stopManual();

            break;
        }

        case TPMS_PHONE_CMD_RESULT_CLEAR_LEARN:
        {
            /*
             * 清除学习结果。
             *
             * 当前第一版只清 RAM 里的学习状态。
             * 后续如果保存到 Flash，这里还要调用 Flash 擦除接口。
             */
            TpmsLearning_init();

            break;
        }

        case TPMS_PHONE_CMD_RESULT_ERROR:
        default:
        {
            break;
        }
    }
}


/*
 * ============================================================
 * 发送测试 FFB2 报文
 * ============================================================
 *
 * 主要用于：
 *
 * 1. App 连接握手测试
 * 2. FFB1 = 01 唤醒命令测试
 *
 * 注意：
 * 手动学习成功帧不从这里发。
 * 手动学习成功帧由 tpms_learning.c 发。
 */

static void TpmsApp_sendTestFfb2Report(uint8_t position)
{
    uint8_t sensor_id[4];
    uint8_t info;

    /*
     * 测试 ID：
     * 12 34 56 78
     */
    sensor_id[0] = 0x12U;
    sensor_id[1] = 0x34U;
    sensor_id[2] = 0x56U;
    sensor_id[3] = 0x78U;

    /*
     * Info:
     *
     * Vendor = Sensata
     * Mode = Running
     * Battery = Normal
     * LF = Triggered
     *
     * 按你之前定义，生成结果一般是 0x25。
     */
    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_RUNNING,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_TRIGGERED);

    /*
     * FFB2 12 字节格式：
     *
     * Byte0  = 00
     * Byte1  = 01
     * Byte2  = ID0
     * Byte3  = ID1
     * Byte4  = ID2
     * Byte5  = ID3
     * Byte6  = PressureRaw
     * Byte7  = TemperatureRaw
     * Byte8  = Info
     * Byte9  = FunctionReuse0
     * Byte10 = FunctionReuse1
     * Byte11 = Position
     *
     * 这里 PressureRaw / TemperatureRaw 用之前测试值：
     *
     * 0xC7 -> 约 273 kPa
     * 0x38 -> 约 6℃
     */
    (void)TpmsBleService_notifyFfb2SensorRaw(sensor_id,
                                            0xC7U,
                                            0x38U,
                                            info,
                                            TPMS_APP_FUNC_REUSE_DEFAULT_0,
                                            TPMS_APP_FUNC_REUSE_DEFAULT_1,
                                            position);
}
