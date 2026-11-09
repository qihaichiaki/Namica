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

更新：2026-11-09。这里只保留最新接续状态，详细事实与依据由上方主题文件维护。

- **当前重点（已确认）**：当前阶段使用 C++ 标准库实现日志，公共接口只使用 Namica/标准类型；保留 LogRecord/LogSink 分离，将整行布局和最低等级迁移至输出端，暂不开发多线程日志，见 [项目决定](DECISIONS.md#已确认)。文本输出端由 TextSink 统一布局与日期格式，ConsoleSink 和 FileSink 共享 `setLogFmt`/`setDateFmt`/`formatRecord`；布局为 `{0}` 时间、`{1}` 等级、`{2}` 来源、`{3}` 正文。
- **实现进度**：FileSink 已完成基本打开、写入与刷新，并接入 LogConfig/Log::init；Core/App 共享同一个默认控制台端及可选文件端。包含对象约束和打开模式校验的当前版本已通过 Debug 构建检查及日志测试 33/33，编辑器冒烟确认共享文件输出为 UTF-8/LF。尚无 FileSink 专项单测，详见 [PROJECT](PROJECT.md)。
- **停留位置**：FileSink/ConsoleSink 已禁止复制与移动，FileSink 已在打开前拒绝无效模式；默认日志路径、FileSystem 接口兼容和专项测试边界见 PROJECT。逐条开关及后续增量扩展原则见 [刷新策略与兼容边界](ARCHITECTURE.md#文件刷新策略与接口稳定性待讨论)。
- **下一步（建议，未授权实施）**：收敛上述边界并补 FileSink 专项回归；周期调度等到主循环具备维护阶段时再接入。Logger 错误收集、初始化回滚、析构失败边界与真实控制台编码仍按后续阶段处理。

跨会话、跨电脑接续以读取同一份最新 `.agent/` 文件为前提；未写入文件的其他会话内容无法据此恢复，文件本身不提供自动传输。接续时核对相关源码，维护当前结论，不积累已经失效且无项目影响的讨论历史。
