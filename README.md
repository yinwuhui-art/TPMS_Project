# CC2745 BLE_basic 工程（v1）

## 📌 项目简介

本工程基于 TI（Texas Instruments）CCS 示例工程 **BLE_basic** 修改而来，运行平台为 **CC2745R10 + FreeRTOS**。

当前版本（v1）为原始基础版本，主要用于 BLE 协议学习与二次开发的起点工程。

---

## 🧩 芯片与开发环境

- 芯片型号：CC2745R10
- SDK：TI SimpleLink CC27xx SDK
- IDE：Code Composer Studio (CCS)
- 操作系统：FreeRTOS
- 协议栈：TI BLE Stack

---

## 🚀 当前版本功能（v1）

v1 为示例工程原始稳定版本，包含以下基础功能：

- BLE 基本初始化
- GAP 广播功能
- GATT 服务框架
- 基础连接流程
- 示例 Profile（读/写/Notify）
- FreeRTOS 基础任务调度

---

## 📡 BLE 功能说明

本工程基于 TI 官方 BLE_basic 示例，默认支持：

- BLE 广播
- 设备可被扫描与连接
- 基础 GATT 通信
- 示例特征值读写/通知

---

## 📁 工程结构说明
basic_ble/
├── app/ # 应用层代码
├── common/ # 公共模块
├── main_freertos.c # 主入口
├── basic_ble.syscfg # SysConfig配置
├── targetConfigs/ # CCS目标配置
├── Release/ # 编译输出

---

## 🧠 v1版本说明

v1 作为最初版本，特点如下：

- 基于 TI 官方 BLE_basic 示例
- 未进行业务逻辑修改
- 仅用于验证环境与基础通信
- 作为后续 TPMS / BLE 功能开发基线

---

## 🔄 后续计划（Roadmap）

- v2：加入自定义 BLE 通信协议
- v3：接入 TPMS 数据解析逻辑
- v4：优化数据结构与低功耗模式
- v5：对接手机 App 数据交互

---

## 📌 使用说明

1. 使用 CCS 导入工程
2. 编译工程（Build Project）
3. 烧录到 CC2745 开发板
4. 使用 BLE 调试工具扫描设备

---

## ⚠️ 注意事项

- 本工程基于 TI 示例代码，仅用于学习与开发
- 编译输出文件（Debug/Release）已被 Git 忽略
- 不建议直接修改底层 BLE Stack 文件

---

## 📜 版本记录

| 版本 | 说明 |
|------|------|
| v1 | 基于 BLE_basic 示例的初始工程 |

---

## 👨‍💻 作者

基于 TI CCS 示例工程修改与整理