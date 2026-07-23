/*
 * tpms_learning.h
 *
 * TPMS manual learning module
 */

#ifndef TPMS_LEARNING_H_
#define TPMS_LEARNING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * 四个轮位：
 *
 * index 0 = LF 左前
 * index 1 = RF 右前
 * index 2 = RR 右后
 * index 3 = LR 左后
 */
#define TPMS_LEARNING_WHEEL_COUNT            (4U)

/*
 * 学习结果
 */
#define TPMS_LEARNING_RESULT_NONE            (0U)
#define TPMS_LEARNING_RESULT_STARTED         (1U)
#define TPMS_LEARNING_RESULT_SUCCESS         (2U)
#define TPMS_LEARNING_RESULT_DUPLICATE       (3U)
#define TPMS_LEARNING_RESULT_IGNORED         (4U)
#define TPMS_LEARNING_RESULT_FINISHED        (5U)
#define TPMS_LEARNING_RESULT_ERROR           (255U)

/*
 * Manufacturer Data 中 FunctionReuse 字节位置。
 *
 * 当前按之前抓到的格式：
 *
 * FF FF 00 03 00 03
 *
 * mfr_data[4] = FunctionReuse0
 * mfr_data[5] = FunctionReuse1
 *
 * 文档要求：
 * FunctionReuse 字节 bit1 = 1
 *
 * 当前用 FunctionReuse1 的 bit1 判断。
 */
#define TPMS_LEARNING_FUNCTION_REUSE0_INDEX  (4U)
#define TPMS_LEARNING_FUNCTION_REUSE1_INDEX  (5U)
#define TPMS_LEARNING_TRIGGER_BIT_MASK       (0x02U)


/*
 * ============================================================
 * 学习模式状态变量
 * ============================================================
 */

extern volatile uint8_t  g_tpms_learning_mode_active;
extern volatile uint8_t  g_tpms_learning_step_index;
extern volatile uint8_t  g_tpms_learning_learned_count;
extern volatile uint8_t  g_tpms_learning_done_flags;
extern volatile uint8_t  g_tpms_learning_last_result;
extern volatile uint8_t  g_tpms_learning_last_position;


/*
 * ============================================================
 * 四个轮位学习白名单
 * ============================================================
 *
 * index 0 = LF
 * index 1 = RF
 * index 2 = RR
 * index 3 = LR
 */

extern volatile uint8_t  g_tpms_learning_white_valid[TPMS_LEARNING_WHEEL_COUNT];
extern volatile uint8_t  g_tpms_learning_white_position[TPMS_LEARNING_WHEEL_COUNT];
extern volatile uint8_t  g_tpms_learning_white_addr[TPMS_LEARNING_WHEEL_COUNT][6];
extern volatile uint8_t  g_tpms_learning_white_id[TPMS_LEARNING_WHEEL_COUNT][4];

extern volatile int8_t   g_tpms_learning_white_rssi[TPMS_LEARNING_WHEEL_COUNT];
extern volatile uint8_t  g_tpms_learning_white_addr_type[TPMS_LEARNING_WHEEL_COUNT];
extern volatile uint8_t  g_tpms_learning_white_function_reuse[TPMS_LEARNING_WHEEL_COUNT][2];


/*
 * ============================================================
 * 最近一次学习结果
 * ============================================================
 */

extern volatile uint8_t  g_tpms_learning_last_addr[6];
extern volatile uint8_t  g_tpms_learning_last_id[4];
extern volatile int8_t   g_tpms_learning_last_rssi;
extern volatile uint8_t  g_tpms_learning_last_addr_type;
extern volatile uint8_t  g_tpms_learning_last_function_reuse[2];


/*
 * ============================================================
 * 对外接口
 * ============================================================
 */

void TpmsLearning_init(void);

void TpmsLearning_startManual(void);

void TpmsLearning_stopManual(void);

bool TpmsLearning_isActive(void);

/*
 * 扫描模块识别到目标 TPMS 后调用。
 *
 * whitelist_index：
 *
 * 0 = LF 左前
 * 1 = RF 右前
 * 2 = RR 右后
 * 3 = LR 左后
 */
void TpmsLearning_onTpmsAdv(const uint8_t *addr,
                            uint8_t addr_type,
                            int8_t rssi,
                            uint8_t whitelist_index,
                            const uint8_t *mfr_data,
                            uint8_t mfr_len);

#ifdef __cplusplus
}
#endif

#endif /* TPMS_LEARNING_H_ */
