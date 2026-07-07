/*
 * tpms_log.h
 *
 *  Created on: 2026年7月2日
 *      Author: yinwuhui
 */

#ifndef TPMS_LOG_H_
#define TPMS_LOG_H_

/*
 * 第一阶段先不强依赖 UART / Display / Log 模块。
 * 后面你确定串口输出方式后，再在这里统一封装。
 */

#define TPMS_LOG_INFO(...)
#define TPMS_LOG_WARN(...)
#define TPMS_LOG_ERROR(...)

#endif
