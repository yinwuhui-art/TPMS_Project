#include "tpms_learning.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "tpms_types.h"
#include "tpms_protocol.h"

#include "../ble/tpms_ble_service.h"



/*************************************************
 * Learning status
 *************************************************/

volatile uint8_t g_tpms_learning_mode_active = 0;

volatile uint8_t g_tpms_learning_step_index = 0;

volatile uint8_t g_tpms_learning_learned_count = 0;

volatile uint8_t g_tpms_learning_done_flags = 0;


volatile uint8_t g_tpms_learning_last_result =
        TPMS_LEARNING_RESULT_IDLE;


volatile uint8_t g_tpms_learning_last_success_result =
        TPMS_LEARNING_RESULT_IDLE;


volatile uint8_t g_tpms_learning_last_position = 0;


volatile uint8_t g_tpms_learning_last_id[4] =
{
    0,0,0,0
};


volatile uint8_t g_tpms_learning_last_function_reuse[2] =
{
    0,0
};



/*
 * 四个轮胎学习状态
 *
 * 0 LF
 * 1 RF
 * 2 RR
 * 3 LR
 */

volatile uint8_t g_tpms_learning_wheel_learn_flag[4] =
{
    0,0,0,0
};



/*
 * 记录实际学习到的传感器
 */

volatile uint8_t g_tpms_learning_white_source_index[4] =
{
    0xFF,
    0xFF,
    0xFF,
    0xFF
};



static uint8_t s_learning_mac[4][6];


static uint8_t s_learning_id[4][4];



/*
 * 学习顺序
 *
 * LF RF RR LR
 */

static const uint8_t s_learning_position_table[4]=
{
    TPMS_APP_POS_LF,
    TPMS_APP_POS_RF,
    TPMS_APP_POS_RR,
    TPMS_APP_POS_LR
};



/*************************************************
 * 初始化
 *************************************************/

void TpmsLearning_init(void)
{
    uint8_t i;


    g_tpms_learning_mode_active = 0;

    g_tpms_learning_step_index = 0;

    g_tpms_learning_learned_count = 0;

    g_tpms_learning_done_flags = 0;


    g_tpms_learning_last_result =
            TPMS_LEARNING_RESULT_IDLE;


    g_tpms_learning_last_success_result =
            TPMS_LEARNING_RESULT_IDLE;


    g_tpms_learning_last_position = 0;


    memset((void *)g_tpms_learning_last_id,
           0,
           sizeof(g_tpms_learning_last_id));


    memset((void *)g_tpms_learning_last_function_reuse,
           0,
           sizeof(g_tpms_learning_last_function_reuse));


    memset((void *)g_tpms_learning_wheel_learn_flag,
           0,
           sizeof(g_tpms_learning_wheel_learn_flag));


    for(i=0;i<4;i++)
    {
        g_tpms_learning_white_source_index[i]=0xFF;
    }


    memset(s_learning_mac,0,sizeof(s_learning_mac));

    memset(s_learning_id,0,sizeof(s_learning_id));

}



/*************************************************
 * 开始学习
 *************************************************/

void TpmsLearning_startManual(void)
{
    TpmsLearning_init();


    g_tpms_learning_mode_active = 1;


    g_tpms_learning_step_index = 0;


    g_tpms_learning_last_result =
            TPMS_LEARNING_RESULT_STARTED;

}



/*************************************************
 * 停止学习
 *************************************************/

void TpmsLearning_stopManual(void)
{
    g_tpms_learning_mode_active=0;
}



bool TpmsLearning_isActive(void)
{
    return (g_tpms_learning_mode_active != 0);
}



/*************************************************
 * FunctionReuse bit1 判断
 *************************************************/

static bool TpmsLearning_checkFunctionReuse(
        const uint8_t *mfr,
        uint8_t len)
{

    if(mfr == NULL)
        return false;


    if(len < 6)
        return false;



    /*
     * Byte5 bit1
     *
     * 0x02
     *
     */

    if((mfr[5] & 0x02U)!=0)
    {
        return true;
    }


    return true;
}



/*************************************************
 * 判断是否已经学习
 *************************************************/

static bool TpmsLearning_isDuplicate(
        uint8_t source_index)
{
    uint8_t i;


    for(i=0;i<4;i++)
    {
        if(g_tpms_learning_white_source_index[i]
                == source_index)
        {
            return true;
        }
    }


    return false;
}



/*************************************************
 * MAC转ID
 *************************************************/

static void TpmsLearning_makeId(
        const uint8_t *addr,
        uint8_t *id)
{

    /*
     * BLE地址：
     *
     * D0:39:3F:19:83:06
     *
     * TI顺序：
     *
     * 06 83 19 3F 39 D0
     *
     * ID:
     *
     * 3F 19 83 06
     */


    id[0]=addr[3];
    id[1]=addr[2];
    id[2]=addr[1];
    id[3]=addr[0];

}



/*************************************************
 * 设置累计学习标志
 *************************************************/

static void TpmsLearning_setPositionFlag(
        uint8_t position)
{

    /*
     *
     * LF  =0x01
     * RF  =0x02
     * RR  =0x08
     * LR  =0x04
     *
     */


    g_tpms_learning_done_flags |= position;

}



/*************************************************
 * 发送APP
 *************************************************/

static void TpmsLearning_notifyApp(
        uint8_t wheel_index)
{

    uint8_t info;


    info =
        TpmsProtocol_makeInfo(
            TPMS_APP_VENDOR_SENSATA,
            TPMS_APP_MODE_RUNNING,
            TPMS_APP_BATTERY_NORMAL,
            TPMS_APP_LF_NOT_TRIGGERED);



    /*
     * 关键修改：
     *
     * 使用队列+重发
     *
     */

    TpmsBleService_queueFfb2SensorRaw(

        s_learning_id[wheel_index],

        0,

        0,

        info,

        0,

        0,

        g_tpms_learning_done_flags,

        3U
    );

}

/*************************************************
 * 清除单个旧学习记录
 *
 * App 协议要求：
 *
 * 旧 Sensor ID + position = 0x00
 *
 * App 收到后，把这个旧 Sensor ID 对应的轮胎学习记录清掉。
 *************************************************/

static void TpmsLearning_sendClearRecordToApp(uint8_t wheel_index)
{
    uint8_t info;

    if (wheel_index >= 4U)
    {
        return;
    }

    /*
     * 这个轮位没有学习过，不需要通知 App 清除。
     */
    if (g_tpms_learning_wheel_learn_flag[wheel_index] == 0U)
    {
        return;
    }

    info =
        TpmsProtocol_makeInfo(
            TPMS_APP_VENDOR_SENSATA,
            TPMS_APP_MODE_RUNNING,
            TPMS_APP_BATTERY_NORMAL,
            TPMS_APP_LF_NOT_TRIGGERED);

    /*
     * 重点：
     *
     * Byte2~Byte5 = 旧 Sensor ID
     * Byte11       = 0x00
     *
     * 这里使用队列发送，重复 2 次，提高 App 收到清除帧的概率。
     */
    (void)TpmsBleService_queueFfb2SensorRaw(
        s_learning_id[wheel_index],
        0U,
        0U,
        info,
        0U,
        0U,
        TPMS_APP_POS_NONE,
        2U);
}


/*************************************************
 * 重新学习前，通知 App 清除上一轮学习记录
 *************************************************/

void TpmsLearning_notifyClearLearnedRecords(void)
{
    uint8_t i;

    /*
     * 注意：
     *
     * 这个函数必须在 TpmsLearning_startManual() 之前调用。
     *
     * 因为 TpmsLearning_startManual() 会调用 TpmsLearning_init()，
     * 会清空 s_learning_id / g_tpms_learning_wheel_learn_flag。
     */
    for (i = 0U; i < 4U; i++)
    {
        if (g_tpms_learning_wheel_learn_flag[i] != 0U)
        {
            TpmsLearning_sendClearRecordToApp(i);
        }
    }
}

/*************************************************
 * 核心学习
 *************************************************/

void TpmsLearning_onTpmsAdv(
        uint8_t whitelist_index,
        const uint8_t *addr,
        uint8_t addr_type,
        int8_t rssi,
        const uint8_t *mfr_data,
        uint8_t mfr_len)
{


    uint8_t wheel_index;

    uint8_t position;

    uint8_t id[4];



    if(g_tpms_learning_mode_active==0)
        return;



    wheel_index =
        g_tpms_learning_step_index;



    if(wheel_index>=4)
        return;



    /*
     * 必须低频触发
     */

    if(TpmsLearning_checkFunctionReuse(
            mfr_data,
            mfr_len)==false)
    {

        /*
         * 不覆盖成功结果
         */

        if(g_tpms_learning_learned_count<4)
        {
            g_tpms_learning_last_result =
                    TPMS_LEARNING_RESULT_IGNORE;
        }

        return;
    }



    /*
     * 已经学习过
     */

    if(TpmsLearning_isDuplicate(
            whitelist_index))
    {

        g_tpms_learning_last_result =
                TPMS_LEARNING_RESULT_DUPLICATE;

        return;
    }



    /*
     * 保存来源
     */

    g_tpms_learning_white_source_index[wheel_index]
            =
            whitelist_index;



    memcpy(
        s_learning_mac[wheel_index],
        addr,
        6);



    /*
     * 生成ID
     */

    TpmsLearning_makeId(
        addr,
        id);



    memcpy(
        s_learning_id[wheel_index],
        id,
        4);



    memcpy(
        (void *)g_tpms_learning_last_id,
        id,
        4);



    position =
        s_learning_position_table[wheel_index];



    g_tpms_learning_last_position =
            position;



    g_tpms_learning_last_function_reuse[0]
            =
            mfr_data[4];


    g_tpms_learning_last_function_reuse[1]
            =
            mfr_data[5];



    /*
     * 当前轮学习完成
     */

    g_tpms_learning_wheel_learn_flag[wheel_index]
            =1;



    /*
     * 累计置位
     *
     * LF
     * LF+RF
     * LF+RF+RR
     * LF+RF+RR+LR
     */

    TpmsLearning_setPositionFlag(position);



    g_tpms_learning_learned_count++;



    /*
     * 通知APP
     */

    TpmsLearning_notifyApp(
            wheel_index);



    g_tpms_learning_last_success_result =
            TPMS_LEARNING_RESULT_SUCCESS;


    g_tpms_learning_last_result =
            TPMS_LEARNING_RESULT_SUCCESS;



    /*
     * 下一轮
     */

    g_tpms_learning_step_index++;



    if(g_tpms_learning_step_index>=4)
    {

        g_tpms_learning_mode_active=0;


        g_tpms_learning_last_result =
                TPMS_LEARNING_RESULT_FINISHED;

    }

}
