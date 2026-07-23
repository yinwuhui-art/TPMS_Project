/*
 * tpms_learning.c
 *
 * TPMS manual learning module
 */

#include "tpms_learning.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../tpms_config.h"
#include "../phone/tpms_protocol.h"
#include "../ble/tpms_ble_service.h"


/*
 * ============================================================
 * 全局调试变量
 * ============================================================
 */

volatile uint8_t  g_tpms_learning_mode_active = 0U;
volatile uint8_t  g_tpms_learning_step_index = 0U;
volatile uint8_t  g_tpms_learning_learned_count = 0U;
volatile uint8_t  g_tpms_learning_done_flags = 0U;
volatile uint8_t  g_tpms_learning_last_result = TPMS_LEARNING_RESULT_NONE;
volatile uint8_t  g_tpms_learning_last_position = TPMS_APP_POS_NONE;

volatile uint8_t  g_tpms_learning_white_valid[TPMS_LEARNING_WHEEL_COUNT] = {0};
volatile uint8_t  g_tpms_learning_white_position[TPMS_LEARNING_WHEEL_COUNT] = {0};
volatile uint8_t  g_tpms_learning_white_addr[TPMS_LEARNING_WHEEL_COUNT][6] = {{0}};
volatile uint8_t  g_tpms_learning_white_id[TPMS_LEARNING_WHEEL_COUNT][4] = {{0}};

volatile int8_t   g_tpms_learning_white_rssi[TPMS_LEARNING_WHEEL_COUNT] = {0};
volatile uint8_t  g_tpms_learning_white_addr_type[TPMS_LEARNING_WHEEL_COUNT] = {0};
volatile uint8_t  g_tpms_learning_white_function_reuse[TPMS_LEARNING_WHEEL_COUNT][2] = {{0}};

volatile uint8_t g_tpms_learning_wheel_learn_flag[TPMS_LEARNING_WHEEL_COUNT] =
{
    0U,
    0U,
    0U,
    0U
};

volatile uint8_t g_tpms_learning_white_source_index[TPMS_LEARNING_WHEEL_COUNT] =
{
    0xFFU,
    0xFFU,
    0xFFU,
    0xFFU
};

volatile uint8_t g_tpms_learning_last_whitelist_index = 0xFFU;

volatile uint8_t  g_tpms_learning_last_addr[6] = {0};
volatile uint8_t  g_tpms_learning_last_id[4] = {0};
volatile int8_t   g_tpms_learning_last_rssi = 0;
volatile uint8_t  g_tpms_learning_last_addr_type = 0U;
volatile uint8_t  g_tpms_learning_last_function_reuse[2] = {0};


/*
 * ============================================================
 * 内部辅助函数
 * ============================================================
 */

/*
 * 根据白名单 index 获取 FFB2 Byte11 position。
 *
 * 白名单 index：
 * 0 = LF
 * 1 = RF
 * 2 = RR
 * 3 = LR
 *
 * FFB2 position：
 * LF = 0x01
 * RF = 0x02
 * RR = 0x08
 * LR = 0x04
 */
static uint8_t TpmsLearning_positionFromWhitelistIndex(uint8_t whitelist_index)
{
    uint8_t position;

    switch (whitelist_index)
    {
        case 0U:
            position = TPMS_APP_POS_LF;
            break;

        case 1U:
            position = TPMS_APP_POS_RF;
            break;

        case 2U:
            position = TPMS_APP_POS_RR;
            break;

        case 3U:
            position = TPMS_APP_POS_LR;
            break;

        default:
            position = TPMS_APP_POS_NONE;
            break;
    }

    return position;
}


/*
 * 根据白名单 index 生成 4 字节 Sensor ID。
 *
 * 当前 Sensor ID 先用 MAC 地址后 4 字节：
 *
 * LF: D0:39:3F:19:83:06 -> 3F 19 83 06
 * RF: D0:39:3F:18:68:D3 -> 3F 18 68 D3
 * RR: D0:39:3F:19:27:31 -> 3F 19 27 31
 * LR: D0:39:3F:17:BA:01 -> 3F 17 BA 01
 *
 * 这样不依赖 TI 回调里地址是正序还是反序。
 */
static bool TpmsLearning_makeSensorIdFromWhitelistIndex(uint8_t whitelist_index,
                                                        uint8_t *sensor_id)
{
    static const uint8_t id_table[TPMS_LEARNING_WHEEL_COUNT][4] =
    {
        {
            TPMS_SENSOR_LF_ADDR_2,
            TPMS_SENSOR_LF_ADDR_3,
            TPMS_SENSOR_LF_ADDR_4,
            TPMS_SENSOR_LF_ADDR_5
        },
        {
            TPMS_SENSOR_RF_ADDR_2,
            TPMS_SENSOR_RF_ADDR_3,
            TPMS_SENSOR_RF_ADDR_4,
            TPMS_SENSOR_RF_ADDR_5
        },
        {
            TPMS_SENSOR_RR_ADDR_2,
            TPMS_SENSOR_RR_ADDR_3,
            TPMS_SENSOR_RR_ADDR_4,
            TPMS_SENSOR_RR_ADDR_5
        },
        {
            TPMS_SENSOR_LR_ADDR_2,
            TPMS_SENSOR_LR_ADDR_3,
            TPMS_SENSOR_LR_ADDR_4,
            TPMS_SENSOR_LR_ADDR_5
        }
    };

    if (sensor_id == NULL)
    {
        return false;
    }

    if (whitelist_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        return false;
    }

    memcpy(sensor_id,
           id_table[whitelist_index],
           4U);

    return true;
}


/*
 * 判断 FunctionReuse bit1 是否为 1。
 *
 * 当前规则：
 * mfr_data[5] & 0x02 != 0
 */
static bool TpmsLearning_isFunctionReuseBit1Set(const uint8_t *mfr_data,
                                                uint8_t mfr_len)
{
    uint8_t function_reuse;

    if (mfr_data == NULL)
    {
        return false;
    }

    if (mfr_len <= TPMS_LEARNING_FUNCTION_REUSE1_INDEX)
    {
        return false;
    }

    function_reuse = mfr_data[TPMS_LEARNING_FUNCTION_REUSE1_INDEX];

    if ((function_reuse & TPMS_LEARNING_TRIGGER_BIT_MASK) != 0U)
    {
        return true;
    }

    return false;
}

//7.23add
static void TpmsLearning_setDoneFlag(uint8_t wheel_index, uint8_t position)
{
    if (wheel_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        return;
    }

    /*
     * position 是 Byte11 的轮位 bit：
     *
     * LF = 0x01
     * RF = 0x02
     * LR = 0x04
     * RR = 0x08
     *
     * 旧的学习标志保持 1，新的也置 1。
     */
    g_tpms_learning_done_flags |= position;

    /*
     * 独立的四轮学习标志：
     *
     * [1,0,0,0] = LF 完成
     * [1,1,0,0] = LF、RF 完成
     * [1,1,1,0] = LF、RF、RR 完成
     * [1,1,1,1] = 四轮完成
     */
    g_tpms_learning_wheel_learn_flag[wheel_index] = 1U;
}

//7.23清除旧学习记录通知函数
static void TpmsLearning_sendClearLearnRecordReport(uint8_t wheel_index)
{
    uint8_t info;

    if (wheel_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        return;
    }

    if (g_tpms_learning_white_valid[wheel_index] == 0U)
    {
        return;
    }

    /*
     * 清除旧学习记录。
     *
     * 重点：
     *
     * Byte2~Byte5 = 旧 Sensor ID
     * Byte11 position = 0x00
     *
     * App 收到后，应清除这个旧 Sensor ID 对应的学习记录。
     */
    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_LOCALIZATION,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_NOT_TRIGGERED);

    (void)TpmsBleService_notifyFfb2SensorRaw(
        (const uint8_t *)g_tpms_learning_white_id[wheel_index],
        0x00U,
        0x00U,
        info,
        0x00U,
        0x00U,
        TPMS_APP_POS_NONE);
}


void TpmsLearning_notifyClearLearnedRecords(void)
{
    uint8_t i;

    /*
     * 注意：
     * 这个函数必须在 TpmsLearning_startManual() 之前调用。
     *
     * 因为 TpmsLearning_startManual() 会调用 TpmsLearning_init()，
     * 会把上一轮学习记录清掉。
     */
    for (i = 0U; i < TPMS_LEARNING_WHEEL_COUNT; i++)
    {
        if (g_tpms_learning_white_valid[i] != 0U)
        {
            TpmsLearning_sendClearLearnRecordReport(i);
        }
    }
}

/*
 * 学习成功后通过 FFB2 Notify 通知 App。
 *
 * 学习阶段主要让 App 知道：
 * 1. 哪个轮位学习完成
 * 2. 学到的 Sensor ID 是多少
 *
 * 当前还没有在 BLEM 里连接 SNP756 读取 05000200 Indicate，
 * 所以 pressure / temperature 先填 0。
 */
static void TpmsLearning_sendLearnDoneReport(uint8_t wheel_index)
{
    uint8_t info;
    uint8_t pressure_raw;
    uint8_t temperature_raw;

    if (wheel_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        return;
    }

    pressure_raw = 0x00U;
    temperature_raw = 0x00U;

    info = TpmsProtocol_makeInfo(TPMS_APP_VENDOR_SENSATA,
                                 TPMS_APP_MODE_LOCALIZATION,
                                 TPMS_APP_BATTERY_NORMAL,
                                 TPMS_APP_LF_TRIGGERED);

    (void)TpmsBleService_notifyFfb2SensorRaw(
        (const uint8_t *)g_tpms_learning_white_id[wheel_index],
        pressure_raw,
        temperature_raw,
        info,
        g_tpms_learning_white_function_reuse[wheel_index][0],
        g_tpms_learning_white_function_reuse[wheel_index][1],
        g_tpms_learning_white_position[wheel_index]);
}


/*
 * ============================================================
 * 对外接口
 * ============================================================
 */

void TpmsLearning_init(void)
{
    g_tpms_learning_mode_active = 0U;
    g_tpms_learning_step_index = 0U;
    g_tpms_learning_learned_count = 0U;
    g_tpms_learning_done_flags = 0U;
    g_tpms_learning_last_result = TPMS_LEARNING_RESULT_NONE;
    g_tpms_learning_last_position = TPMS_APP_POS_NONE;

    /*
       * 最近一次物理传感器白名单 index 清空
       */
      g_tpms_learning_last_whitelist_index = 0xFFU;

      /*
       * 四个轮位学习标志清空
       *
       * [0,0,0,0] = 四个轮位均未学习
       */
      memset((void *)g_tpms_learning_wheel_learn_flag,
             0,
             sizeof(g_tpms_learning_wheel_learn_flag));

      /*
       * 每个学习轮位对应的物理传感器来源清空
       */
      g_tpms_learning_white_source_index[0] = 0xFFU;
      g_tpms_learning_white_source_index[1] = 0xFFU;
      g_tpms_learning_white_source_index[2] = 0xFFU;
      g_tpms_learning_white_source_index[3] = 0xFFU;

    memset((void *)g_tpms_learning_white_valid,
           0,
           sizeof(g_tpms_learning_white_valid));

    memset((void *)g_tpms_learning_white_position,
           0,
           sizeof(g_tpms_learning_white_position));

    memset((void *)g_tpms_learning_white_addr,
           0,
           sizeof(g_tpms_learning_white_addr));

    memset((void *)g_tpms_learning_white_id,
           0,
           sizeof(g_tpms_learning_white_id));

    memset((void *)g_tpms_learning_white_rssi,
           0,
           sizeof(g_tpms_learning_white_rssi));

    memset((void *)g_tpms_learning_white_addr_type,
           0,
           sizeof(g_tpms_learning_white_addr_type));

    memset((void *)g_tpms_learning_white_function_reuse,
           0,
           sizeof(g_tpms_learning_white_function_reuse));

    memset((void *)g_tpms_learning_last_addr,
           0,
           sizeof(g_tpms_learning_last_addr));

    memset((void *)g_tpms_learning_last_id,
           0,
           sizeof(g_tpms_learning_last_id));

    memset((void *)g_tpms_learning_last_function_reuse,
           0,
           sizeof(g_tpms_learning_last_function_reuse));

    g_tpms_learning_last_rssi = 0;
    g_tpms_learning_last_addr_type = 0U;
}


void TpmsLearning_startManual(void)
{
    /*
     * 开始一次新的四轮手动学习。
     *
     * 学习顺序：
     *
     * LF → RF → RR → LR
     *
     * 对应 step：
     *
     * step 0 = LF
     * step 1 = RF
     * step 2 = RR
     * step 3 = LR
     */
    TpmsLearning_init();

    g_tpms_learning_mode_active = 1U;
    g_tpms_learning_step_index = 0U;
    g_tpms_learning_learned_count = 0U;
    g_tpms_learning_done_flags = 0U;
    g_tpms_learning_last_result = TPMS_LEARNING_RESULT_STARTED;
    g_tpms_learning_last_position = TPMS_APP_POS_LF;
}


void TpmsLearning_stopManual(void)
{
    g_tpms_learning_mode_active = 0U;
}


bool TpmsLearning_isActive(void)
{
    if (g_tpms_learning_mode_active != 0U)
    {
        return true;
    }

    return false;
}
//7.23add
static bool TpmsLearning_whitelistIndexAlreadyLearned(uint8_t whitelist_index)
{
    uint8_t i;

    if (whitelist_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        return true;
    }

    for (i = 0U; i < TPMS_LEARNING_WHEEL_COUNT; i++)
    {
        if (g_tpms_learning_white_valid[i] != 0U)
        {
            if (g_tpms_learning_white_source_index[i] == whitelist_index)
            {
                return true;
            }
        }
    }

    return false;
}

void TpmsLearning_onTpmsAdv(const uint8_t *addr,
                            uint8_t addr_type,
                            int8_t rssi,
                            uint8_t whitelist_index,
                            const uint8_t *mfr_data,
                            uint8_t mfr_len)
{
    uint8_t wheel_index;
    uint8_t position;
    uint8_t sensor_id[4];

    if ((addr == NULL) || (mfr_data == NULL))
    {
        return;
    }

    /*
     * 不处于学习模式，不处理。
     */
    if (g_tpms_learning_mode_active == 0U)
    {
        return;
    }

    /*
     * 四个轮位都学完，退出学习模式。
     */
    if (g_tpms_learning_step_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        g_tpms_learning_mode_active = 0U;
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_FINISHED;
        return;
    }

    /*
     * 当前扫描到的传感器必须是四个目标白名单传感器之一。
     */
    if (whitelist_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_IGNORED;
        return;
    }

    g_tpms_learning_last_whitelist_index = whitelist_index;

    /*
     * 只学习 FunctionReuse bit1 = 1 的低频触发包。
     *
     * 判断条件：
     *
     * mfr_data[5] & 0x02 != 0
     */
    if (TpmsLearning_isFunctionReuseBit1Set(mfr_data, mfr_len) == false)
    {
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_IGNORED;
        return;
    }

    /*
     * 与历史值不同：
     *
     * 同一个物理传感器不能重复学习。
     *
     * 注意：
     * 这里不用 addr 判断重复。
     * 直接用 whitelist_index 判断重复。
     */
    if (TpmsLearning_whitelistIndexAlreadyLearned(whitelist_index) == true)
    {
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_DUPLICATE;
        return;
    }

    /*
     * 记录到 TPMS 白名单哪个轮胎位置，由学习顺序决定。
     *
     * step 0 = LF 左前
     * step 1 = RF 右前
     * step 2 = RR 右后
     * step 3 = LR 左后
     *
     * whitelist_index 只表示当前实际触发的是哪个物理传感器。
     */
    wheel_index = g_tpms_learning_step_index;

    position = TpmsLearning_positionFromWhitelistIndex(wheel_index);

    if (position == TPMS_APP_POS_NONE)
    {
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_ERROR;
        return;
    }

    memset(sensor_id, 0, sizeof(sensor_id));

    /*
     * Sensor ID 根据实际触发到的物理传感器生成。
     */
    if (TpmsLearning_makeSensorIdFromWhitelistIndex(whitelist_index,
                                                   sensor_id) == false)
    {
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_ERROR;
        return;
    }

    /*
     * 写入 TPMS 学习白名单。
     *
     * 第一次有效低频触发包 -> LF
     * 第二次有效低频触发包 -> RF
     * 第三次有效低频触发包 -> RR
     * 第四次有效低频触发包 -> LR
     */
    g_tpms_learning_white_valid[wheel_index] = 1U;
    g_tpms_learning_white_position[wheel_index] = position;
    g_tpms_learning_white_rssi[wheel_index] = rssi;
    g_tpms_learning_white_addr_type[wheel_index] = addr_type;

    /*
     * 记录当前轮位对应的是哪个物理传感器。
     */
    g_tpms_learning_white_source_index[wheel_index] = whitelist_index;

    /*
     * 保留 MAC 地址，后续真实胎压胎温匹配要用。
     */
    memcpy((void *)g_tpms_learning_white_addr[wheel_index],
           addr,
           6U);

    /*
     * 保存 Sensor ID。
     */
    memcpy((void *)g_tpms_learning_white_id[wheel_index],
           sensor_id,
           4U);

    /*
     * 保存 FunctionReuse。
     */
    g_tpms_learning_white_function_reuse[wheel_index][0] =
        mfr_data[TPMS_LEARNING_FUNCTION_REUSE0_INDEX];

    g_tpms_learning_white_function_reuse[wheel_index][1] =
        mfr_data[TPMS_LEARNING_FUNCTION_REUSE1_INDEX];

    /*
     * 保存最近一次学习结果，方便 CCS Expressions 观察。
     */
    memcpy((void *)g_tpms_learning_last_addr,
           addr,
           6U);

    memcpy((void *)g_tpms_learning_last_id,
           sensor_id,
           4U);

    g_tpms_learning_last_function_reuse[0] =
        mfr_data[TPMS_LEARNING_FUNCTION_REUSE0_INDEX];

    g_tpms_learning_last_function_reuse[1] =
        mfr_data[TPMS_LEARNING_FUNCTION_REUSE1_INDEX];

    g_tpms_learning_last_rssi = rssi;
    g_tpms_learning_last_addr_type = addr_type;
    g_tpms_learning_last_position = position;
    g_tpms_learning_last_result = TPMS_LEARNING_RESULT_SUCCESS;

    /*
     * 已学习数量 +1。
     */
    g_tpms_learning_learned_count++;

    /*
     * 学习标志置位：
     *
     * 旧轮位保持 1，新轮位也置 1。
     */
    TpmsLearning_setDoneFlag(wheel_index, position);

    /*
     * 学习成功后，通过 FFB2 Notify 发给 App。
     *
     * App 根据 Byte11 position 更新 UI：
     *
     * LF = 0x01
     * RF = 0x02
     * RR = 0x08
     * LR = 0x04
     */
    TpmsLearning_sendLearnDoneReport(wheel_index);

    /*
     * 进入下一个学习轮位。
     */
    g_tpms_learning_step_index++;

    /*
     * 四个轮位都学习完成。
     */
    if (g_tpms_learning_step_index >= TPMS_LEARNING_WHEEL_COUNT)
    {
        g_tpms_learning_mode_active = 0U;
        g_tpms_learning_last_result = TPMS_LEARNING_RESULT_FINISHED;
    }
}
