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

更新：2026-11-06。这里只保留最新接续状态，详细事实与依据由上方主题文件维护。

- **当前重点（已确认）**：当前阶段使用 C++ 标准库实现日志，公共接口只使用 Namica/标准类型；保留 LogRecord/LogSink 分离，将整行布局和最低等级迁移至输出端，暂不开发多线程日志，见 [项目决定](DECISIONS.md#已确认)。
- **实现进度**：控制台派发、整条配色、换行/刷新与 Application 清理已接通，Sink 的 write/flush 返回 Bool。日志测试按模块放在 tests/runtime/log，目标为 test_log；Logger/Log 与 ConsoleSink 共 33 项，在 Debug/MSVC/Ninja 下全部通过，具体覆盖与限制见 [PROJECT](PROJECT.md)。
- **停留位置**：本轮已完成用户授权的测试修正与补充，运行库实现保持原样；Logger 错误收集明确以后实现，来源布局字段在 FileSink 阶段再加。已确认与待定错误边界见 [运行期日志失败策略](ARCHITECTURE.md#运行期日志失败策略待讨论)。
- **下一步（建议）**：可以继续讨论 FileSink 的打开/追加/刷新契约，再逐步接入编辑器接收端；错误收集按用户安排后续实现。初始化回滚、析构不抛与真实控制台编码仍待完善，未自动开始新功能开发。

跨会话、跨电脑接续以读取同一份最新 `.agent/` 文件为前提；未写入文件的其他会话内容无法据此恢复，文件本身不提供自动传输。接续时核对相关源码，维护当前结论，不积累已经失效且无项目影响的讨论历史。
