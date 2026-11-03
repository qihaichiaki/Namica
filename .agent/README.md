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

- **当前重点（已确认）**：继续推进 Log/Logger，服务贪吃蛇从编辑器操作到独立运行的阶段目标，见 [项目决定](DECISIONS.md#已确认)。
- **实现进度**：已加入 Core/App Log 入口、借用 Logger 引用、六级便捷宏及 Assert 宏；关闭的 Trace/Debug 与 Assert 均展开为 (void)0。Application 已有正常路径的实例登记、重复创建拒绝、析构复位与静态 get。Debug 构建检查、Logger 17 项回归和编辑器入口运行通过，验证范围见 [PROJECT](PROJECT.md)。
- **停留位置**：已确认 Log 强依赖 Application 初始化；DLL 与调用方分别决定自己的日志/断言开关，PRIVATE 不传播符合当前策略。Assert 已采用不带末尾分号的 do/while 封装；构造失败回滚仍待完善，见 [PROJECT](PROJECT.md#当前待修正项)。
- **下一步（建议，尚未授权改代码）**：先补齐初始化失败时的实例占用与日志状态回滚，再明确 Application 管理下的日志关闭与配置边界；文件输出尚未加入。生命周期讨论见 [ARCHITECTURE](ARCHITECTURE.md#日志生命周期待讨论)。当前 agent 仅审阅、运行现有验证并维护文档。

跨会话、跨电脑接续以读取同一份最新 `.agent/` 文件为前提；未写入文件的其他会话内容无法据此恢复，文件本身不提供自动传输。接续时核对相关源码，维护当前结论，不积累已经失效且无项目影响的讨论历史。
