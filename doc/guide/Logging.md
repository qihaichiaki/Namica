# 日志

`Log` 提供 Core/App 两组入口和便捷宏；`Logger` 格式化正文并同步派发记录。`ConsoleSink` 输出到控制台，`FileSink` 输出到文件，两者共享 `TextSink` 的整行布局和日期格式，各自设置最低等级。控制台端额外控制颜色与输出通道。

## 使用便捷宏

使用默认 Log 前必须完成 Application 构造，无需手动初始化日志：

```cpp
#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/log/Log.h>

int main()
{
    namica::ApplicationConfig config{};
    config.logConfig.consoleSinkConfig.minLevel = namica::LogLevel::Info;
    config.logConfig.consoleSinkConfig.colorMode = namica::ConsoleColorMode::Never;

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

ApplicationConfig.logConfig 中的 `haveConsoleSink` 控制是否创建默认控制台端，默认开启；Core/App 共享同一个端，由 `consoleSinkConfig` 提供以下参数，因此等级与输出设置同时影响两组日志：

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

若要调整应用日志并保留 sink 引用，可将启动配置中的 haveConsoleSink 设为 False，Application 构造完成后自行创建并 addSink。需要 Core/App 分别配置时，为两个 Logger 各注册独立的 ConsoleSink；这些端无法接收注册之前的启动日志。

## 输出到文件

在构造 Application 前启用文件端，即可同时收集 Core/App 和启动日志。以下片段接在 ApplicationConfig 创建之后：

```cpp
config.logConfig.haveFileSink = namica::True;
config.logConfig.fileSinkConfig.filePath = "logs/Namica.log";
config.logConfig.fileSinkConfig.minLevel = namica::LogLevel::Info;
config.logConfig.fileSinkConfig.openMode = namica::FileOpenMode::Append;
config.logConfig.fileSinkConfig.flushOnWrite = namica::True;
```

相对路径以创建 sink 时的进程工作目录为准；缺少的父目录会自动创建。未指定路径时，LogConfig 使用可执行目录下的 `Namica.log`；发布到不可写安装目录时，应由应用指定可写路径。文件端默认关闭。

Append 保留已有内容并追加，Truncate 在创建时清空已有内容。两者均以二进制方式写入传入的 UTF-8 字节，每条追加 LF，不添加 BOM 或控制台颜色。空路径、目录创建失败或打开失败会使初始化抛异常。LogConfig 的文件配置默认包含年月日和时间；独立创建的 FileSinkConfig 继承 TextSinkConfig 的时分秒格式，且需要显式指定路径。

需要独立 Logger 或运行期调整时，可自行创建并保留文件端。以下片段假设已有 logger，并包含 `namica/log/sink/FileSink.h` 和 `namica/log/sink/FileSinkConfig.h`：

```cpp
namica::FileSinkConfig fileConfig{};
fileConfig.filePath = "logs/session.log";
fileConfig.dateFmt = NAMICA_TIME_YMD_HMS;
fileConfig.flushOnWrite = namica::False;
auto fileSink{namica::FileSink::create(fileConfig)};
logger->addSink(fileSink);

logger->log(namica::LogLevel::Info, "场景加载完成");
namica::Bool const flushed{fileSink->flush()};
// flushed 为 False 时由调用方处理写入/刷新失败。
fileSink->setFlushOnWrite(namica::True);
```

多个 Logger 写同一个文件时，应注册同一个 FileSink 对象。移除一个 Logger 的注册不会关闭其他持有者使用的文件；最后一个持有者释放时关闭。Application 默认文件端已采用共享方式；手动添加的端只能收到注册后的记录。

## 文本格式与刷新

正文使用 C++20 格式化语法。外部文本作为参数传入，例如 `logger->log(namica::LogLevel::Error, "{}", message)`，避免把正文中的大括号当作格式语法。

文本输出端的 setLogFmt 设置整行布局，字段如下：

| 占位符 | 内容 |
| --- | --- |
| `{0}` | 按 dateFmt 格式化的时间 |
| `{1}` | 等级名称 |
| `{2}` | 日志来源 |
| `{3}` | 已格式化的正文 |

setDateFmt 设置日期显示格式。两个 setter 由 `TextSink` 提供，校验成功返回 True，无效返回 False 并保留旧值；字符串由 sink 保存。`TextSink::formatRecord(record)` 返回当前布局下的纯文本，不额外添加颜色或行结束符，供自定义文本输出端复用。颜色通过 setColorMode/setColorTheme 调整，当前只支持整条记录着色。

ConsoleSink 在每次 write 末尾追加换行并刷新输出流。FileSink 同样追加换行，默认 flushOnWrite=True，每条写完后刷新；False 只关闭逐条刷新，不启用定时器，也不保证数据留在缓冲中直到手动刷新。需要时调用 fileSink->flush() 或 Logger::flush()；Application 正常关闭也会刷新。setFlushOnWrite 只影响后续写入，切换本身不刷新已有缓冲。

logFmt 和正文末尾通常不再加换行，正文内部换行会保留。Logger::flush 刷新所有已注册端，不创建新记录；刷新不等同于断电持久化。

LogSink 的 write/flush 返回 Bool。ConsoleSink 在刷新后检查流状态；FileSink 关闭逐条刷新时，write 成功仅表示流接收了数据，后续 flush 仍可能失败。False 表示检测到写入或刷新失败，True 不保证用户已看到内容。Logger 当前不向调用方汇报这些结果，接口也尚未保证不抛异常。

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

当前没有文件轮转、周期刷新、内置编辑器接收端或线程安全保证。自定义接收端可派生 LogSink 并调用其 protected 配置构造，实现返回 Bool 的 write/flush。write 收到的记录引用仅在该次调用期间有效；需要保留记录时复制数据，不保存引用。不要在 write/flush 中重入同一 Logger 或修改其接收端注册列表。输出端异常尚未逐端隔离。

内部字符串按 UTF-8 使用，但当前控制台仍通过窄字符标准流输出，没有原生 Unicode 控制台适配；真实 Windows 终端代码页不匹配时中文可能乱码。Never 可关闭 sink 添加的颜色。

NAMICA_CORE_ASSERT(condition) 与 NAMICA_APP_ASSERT(condition) 用于开发诊断。定义 NAMICA_ENABLE_ASSERTS 时，失败记录 Error 并在 Windows 触发调试中断；关闭时不求值 condition。它们不能代替必须执行的错误处理。

公开接口：[Log.h](../../runtime/include/namica/log/Log.h) · [LogConfig.h](../../runtime/include/namica/log/LogConfig.h) · [Logger.h](../../runtime/include/namica/log/Logger.h) · [ConsoleSink.h](../../runtime/include/namica/log/sink/ConsoleSink.h) · [FileSink.h](../../runtime/include/namica/log/sink/FileSink.h) · [FileSinkConfig.h](../../runtime/include/namica/log/sink/FileSinkConfig.h)。返回[用户指南](README.md)。
