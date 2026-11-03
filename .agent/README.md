# Namica Agent 知识库

本目录用于接续指导，维护当前有效信息。指导在对话中进行，对外使用说明位于 [doc/guide](../doc/guide/README.md)。

先阅读 [AGENTS.md](AGENTS.md)，再按任务选择：

| 文件 | 负责的内容 |
| --- | --- |
| [AGENTS.md](AGENTS.md) | 角色、操作边界、建议与测试规则、信息维护规范 |
| [PROJECT.md](PROJECT.md) | 当前实现、源码入口、构建要求和真实验证结果 |
| [DECISIONS.md](DECISIONS.md) | 已确认的项目选择与仍需决定的问题 |
| [ARCHITECTURE.md](ARCHITECTURE.md) | 有效设计思路、近期扩展建议及参考依据 |

## 当前接续

更新：2026-11-03。这里只保留最新接续状态，详细事实与依据由上方主题文件维护。

- **当前重点（已确认）**：讨论日志记录与多目标输出，覆盖控制台编码、颜色与分段、文件以及编辑器接收接口，服务贪吃蛇独立运行目标，见 [项目决定](DECISIONS.md#已确认)。
- **实现进度**：Core/App Log、借用引用和便捷宏已有；ApplicationConfig.logConfig 已传入私有 Log::init，分别设置 Core/App 的布局和最低等级，非法启动布局抛 invalid_argument。Application 不再保存完整配置，在日志初始化成功后登记实例。Debug 构建检查、Logger 17 项回归与编辑器入口运行通过，范围见 [PROJECT](PROJECT.md)。
- **停留位置**：首次 Log 初始化已改为局部创建、配置后发布；复用既有实例的分支仍共享旧对象，边界见 [PROJECT](PROJECT.md#当前待修正项)。当前在讨论 LogRecord 与输出端分离、共享目标端、Windows 控制台/重定向处理，以及编辑器接收队列。Log 依赖 Application、Config 仅用于启动的约定保持不变。
- **下一步（建议，尚未授权改代码）**：确定记录与输出接口，优先评估 spdlog 后端；先实现同步控制台与文件，预留编辑器接收边界。布局配置迁移、输出失败与关闭协议仍待确认，方案见 [近期扩展](ARCHITECTURE.md#log-的近期扩展)。本轮仅讨论、运行现有验证并维护文档。

跨会话、跨电脑接续以读取同一份最新 `.agent/` 文件为前提；未写入文件的其他会话内容无法据此恢复，文件本身不提供自动传输。接续时核对相关源码，维护当前结论，不积累已经失效且无项目影响的讨论历史。
