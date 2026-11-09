# 日志

`Log` 提供 Core/App 两组入口和便捷宏；`Logger` 格式化正文并同步派发记录。`ConsoleSink` 当前负责控制台输出，与后续文件输出端共享 `TextSink` 的整行布局和日期格式，并额外控制颜色与输出通道。目前提供控制台输出。

## 使用便捷宏

使用默认 Log 前必须完成 Application 构造，无需手动初始化日志：

```cpp
#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/log/Log.h>

int main()
{
    namica::ApplicationConfig config{};
    config.logConfig.coreConsoleSinkConfig.minLevel = namica::LogLevel::Warning;
    config.logConfig.appConsoleSinkConfig.minLevel = namica::LogLevel::Info;
    config.logConfig.appConsoleSinkConfig.colorMode = namica::ConsoleColorMode::Never;

    namica::Application app{config};
    NAMICA_APP_INFO("得分：{}", 10);
    NAMICA_APP_WARNING("资源尚未加载：{}", "texture.png");
    app.run();
    return 0;
}
```

引擎内部使用 `NAMICA_CORE_*`，应用代码使用 `NAMICA_APP_*`；均提供 TRACE、DEBUG、INFO、WARNING、ERROR、CRITICAL。默认布局为 `[{0}][{1}]{2}: {3}`，其中 `{2}` 是来源（默认 Core/App），`{3}` 是正文。

`Log::getCoreLogger()` 与 `Log::getAppLogger()` 返回借用的 Logger&，可用于提交日志和注册输出端。不要删除或用智能指针接管它们；初始化前及 Application 销毁后不可访问。Application 正常退出时刷新并释放默认日志持有者。

## 启动配置

ApplicationConfig.logConfig 中的 `haveCoreConsoleSink`、`haveAppConsoleSink` 控制是否创建对应默认控制台端，均默认开启；各自的 `coreConsoleSinkConfig`、`appConsoleSinkConfig` 提供以下参数：

| 字段 | 作用 |
| --- | --- |
| minLevel | 最低接收等级，默认 Trace |
| target | Stdout 或 Stderr，默认 Stdout |
| logFmt | 整条记录布局 |
| dateFmt | 时间点的文本格式，默认 NAMICA_TIME_HMS |
| colorTheme | 按等级指定整条记录的颜色 |
| colorMode | Auto、Always、Never，默认 Auto |

Config 仅用于创建，修改原始 Config 不会改变已创建对象。非法启动布局或日期格式可能使创建抛出格式异常；时间格式化还依赖系统时区。运行期调整需要持有对应 ConsoleSink，并调用其 setter；Logger 本身不再提供布局和最低等级 setter。

Auto 在 Windows 下检测实际标准句柄并尝试启用 VT；重定向时不添加颜色。Always 强制添加颜色表示，Never 始终不添加。

LogConfig 的 coreLoggerSource/appLoggerSource 分别设置记录来源，默认 Core/App；它们不会自动改写控制台的 logFmt 文本。

## 创建并持有控制台输出端

独立 Logger 不依赖 Application，create(name) 可指定记录来源，默认 NamicaLogger；创建后没有输出端，需要显式注册：

```cpp
#include <namica/core/Types.h>
#include <namica/log/Logger.h>
#include <namica/log/sink/ConsoleSink.h>
#include <namica/log/sink/ConsoleSinkConfig.h>

int main()
{
    namica::ConsoleSinkConfig config{};
    config.minLevel = namica::LogLevel::Info;
    config.logFmt = "[{1}] {3}";
    config.colorMode = namica::ConsoleColorMode::Never;

    auto consoleSink{namica::ConsoleSink::create(config)};
    auto logger{namica::Logger::create()};
    logger->addSink(consoleSink);

    logger->log(namica::LogLevel::Info, "得分：{}", namica::Int{10});
    consoleSink->setLogMinLevel(namica::LogLevel::Warning);
    logger->log(namica::LogLevel::Info, "本条不输出");
    logger->log(namica::LogLevel::Warning, "资源尚未加载");
    logger->flush();
    return 0;
}
```

addSink 拒绝空指针和同一 Logger 的重复注册，返回是否添加成功；removeSink 解除注册。多个 Logger 可以持有同一个 sink，共享其配置与输出目标。

若要调整应用日志并保留 sink 引用，可在启动配置中关闭对应默认控制台端，Application 构造完成后自行创建并 addSink；此时该端无法接收注册之前的启动日志。

## 文本格式与刷新

正文使用 C++20 格式化语法。外部文本作为参数传入，例如 `logger->log(namica::LogLevel::Error, "{}", message)`，避免把正文中的大括号当作格式语法。

文本输出端的 setLogFmt 设置整行布局，字段如下：

| 占位符 | 内容 |
| --- | --- |
| `{0}` | 按 dateFmt 格式化的时间 |
| `{1}` | 等级名称 |
| `{2}` | 日志来源 |
| `{3}` | 已格式化的正文 |

setDateFmt 设置日期显示格式。两个 setter 由 `TextSink` 提供，校验成功返回 True，无效返回 False 并保留旧值；字符串由 sink 保存。`TextSink::formatRecord(record)` 返回当前布局下的纯文本行，不含颜色和末尾换行，供自定义文本输出端复用。颜色通过 setColorMode/setColorTheme 调整，当前只支持整条记录着色。

ConsoleSink 在每次 write 末尾追加换行并刷新输出流，logFmt 和正文末尾通常不再加换行。正文内部换行会保留。Logger::flush 刷新所有已注册端，不创建新记录、不清空日志历史，也不等同于文件持久化。

LogSink 的 write/flush 返回 Bool。当前 ConsoleSink 在刷新后检查流状态，True 表示未检测到流失败，False 表示写入或刷新失败；不保证用户已看到内容。Logger 当前不向调用方汇报这些结果，接口也尚未保证不抛异常。

## 日志等级

| 等级 | 常见用途 |
| --- | --- |
| Trace | 高频、细粒度执行过程 |
| Debug | 开发诊断信息 |
| Info | 正常运行的重要事件 |
| Warning | 异常情况，但仍可继续 |
| Error | 某项操作失败 |
| Critical | 严重故障标记 |

等级按表中顺序递增，Logger 只向最低等级不高于当前记录的端派发。Critical 本身不会终止程序。只有至少一个端接收时才格式化正文；当前正文格式化失败会丢弃该条记录。运行期过滤不阻止函数参数求值。

## 编译期开关

`NAMICA_LOG_DEBUG` 控制便捷宏中的 Trace/Debug 调用：

| 调用方编译时的配置 | Trace/Debug 宏行为 |
| --- | --- |
| 定义 `NAMICA_LOG_DEBUG` | 调用 Logger，继续受运行期最低等级控制 |
| 未定义 `NAMICA_LOG_DEBUG` | 不执行日志调用，不求值日志参数 |

当前使用 `#ifdef`，因此定义为 `0` 也会启用。Info 及以上不受此开关影响；直接调用 `Logger::log(LogLevel::Debug, ...)` 也不受它裁剪。

DLL 与应用分别配置自己的开关。DLL 的 PRIVATE 编译定义不会随链接 `Namica` 传播到应用；应用的定义也不会改变 DLL 内已编译的调用。需要调试日志的应用目标可显式配置：

```cmake
target_compile_definitions(MyApp PRIVATE NAMICA_LOG_DEBUG)
```

当前开关与 Debug/Release 构建类型没有自动绑定。关闭分支展开为 `(void)0`，不保留日志参数表达式或执行其副作用；这些调用的 C++ 类型与格式化兼容性需要在启用日志的构建中检查。不要在日志参数中放必须执行的业务操作。

## 使用边界

当前没有文件输出、内置编辑器接收端或线程安全保证。自定义接收端可派生 LogSink 并调用其 protected 配置构造，实现返回 Bool 的 write/flush。write 收到的记录引用仅在该次调用期间有效；需要保留记录时复制数据，不保存引用。不要在 write/flush 中重入同一 Logger 或修改其接收端注册列表。输出端异常尚未逐端隔离。

内部字符串按 UTF-8 使用，但当前控制台仍通过窄字符标准流输出，没有原生 Unicode 控制台适配；真实 Windows 终端代码页不匹配时中文可能乱码。Never 可关闭 sink 添加的颜色。

NAMICA_CORE_ASSERT(condition) 与 NAMICA_APP_ASSERT(condition) 用于开发诊断。定义 NAMICA_ENABLE_ASSERTS 时，失败记录 Error 并在 Windows 触发调试中断；关闭时不求值 condition。它们不能代替必须执行的错误处理。

公开接口：[Log.h](../../runtime/include/namica/log/Log.h) · [LogConfig.h](../../runtime/include/namica/log/LogConfig.h) · [Logger.h](../../runtime/include/namica/log/Logger.h) · [ConsoleSink.h](../../runtime/include/namica/log/sink/ConsoleSink.h)。返回[用户指南](README.md)。
