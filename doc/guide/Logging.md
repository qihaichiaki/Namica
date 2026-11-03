# 日志

`Log` 提供 Core/App 两组日志入口和便捷宏；`Logger` 提供等级过滤、消息格式化与布局设置，当前采用同步控制台输出。

## 使用便捷宏

`Log` 依赖 `Application` 管理初始化，使用便捷宏或访问默认 Logger 前，必须先完成应用构造；无需用户显式调用 init：

```cpp
#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/core/Log.h>

int main()
{
    namica::ApplicationConfig config{};
    config.logConfig.coreConfig.minLevel = namica::LogLevel::Warning;
    config.logConfig.appConfig.minLevel = namica::LogLevel::Info;
    namica::Application app{config};

    NAMICA_APP_INFO("得分：{}", 10);
    NAMICA_APP_WARNING("资源尚未加载：{}", "texture.png");
    app.run();
    return 0;
}
```

引擎内部使用 `NAMICA_CORE_*`，应用代码使用 `NAMICA_APP_*`；两组均提供 `TRACE`、`DEBUG`、`INFO`、`WARNING`、`ERROR`、`CRITICAL`。默认整行布局分别为 `[{0}][{1}]Core: {2}` 与 `[{0}][{1}]App: {2}`。

`Log::getCoreLogger()` 与 `Log::getAppLogger()` 返回借用的 `Logger&`，可以设置对应日志的最低等级和布局。调用者不拥有这些实例，不应删除或用智能指针接管它们。初始化前访问不会自动创建 Logger，也没有空值保护；独立工具可采用下方自行创建 Logger 的方式。

## 启动参数与运行期调整

`ApplicationConfig::logConfig` 中的 `coreConfig` 与 `appConfig` 分别配置两组默认 Logger，每组包含：

| 字段 | 作用 |
| --- | --- |
| `logFmt` | 设置整行布局，格式规则见下方说明 |
| `minLevel` | 设置最低日志等级，默认 `LogLevel::Trace` |

这些参数在应用构造时生效，包括应用自身的启动日志。任一启动布局无效时，应用构造会抛出 `std::invalid_argument`。

Config 只表达启动参数。应用创建后修改原始 Config 不会影响正在运行的系统；运行期修改通过对应 Logger 的接口完成，例如：

```cpp
namica::Log::getAppLogger().setLogMinLevel(namica::LogLevel::Warning);
```

运行期布局调整使用 `setLogFmt()`，返回值与失败行为见下方“调整消息和布局”。创建配置对象时应显式包含 `ApplicationConfig.h`；`Application.h` 只提供配置类型的前置声明。

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

## 单独创建 Logger

无需创建 `Application`，在已链接 `Namica` 的程序中使用：

```cpp
#include <namica/core/Logger.h>
#include <namica/core/Types.h>

int main()
{
    auto logger{namica::Logger::create()};
    logger->setLogMinLevel(namica::LogLevel::Info);
    if (!logger->setLogFmt("[{1}] {2}"))
    {
        return 1;
    }

    logger->log(namica::LogLevel::Info, "得分：{}", namica::Int{10});
    logger->log(namica::LogLevel::Debug, "本条低于阈值");
    logger->log(namica::LogLevel::Warning, "资源尚未加载");
    return 0;
}
```

本例只输出 Info 与 Warning 消息，正文分别为“得分：10”和“资源尚未加载”。`create()` 返回 `SharedPtr<Logger>`，实例由共享所有权管理。

## 选择日志等级

等级从低到高排列如下，默认接收全部等级；`setLogMinLevel` 会过滤低于阈值的消息。

| 等级 | 常见用途 |
| --- | --- |
| `Trace` | 高频、细粒度执行过程 |
| `Debug` | 开发诊断信息 |
| `Info` | 正常运行的重要事件 |
| `Warning` | 异常情况，但仍可继续 |
| `Error` | 某项操作失败 |
| `Critical` | 严重故障标记 |

`Critical` 本身不会终止程序。运行期等级过滤不会阻止调用前的参数求值；只有上文关闭的 Trace/Debug 宏会跳过参数求值。

## 调整消息和布局

`log` 的格式串描述正文，遵循 C++20 格式化语法。外部文本应作为参数传入，例如 `logger->log(namica::LogLevel::Error, "{}", message);`，这样正文中的大括号不会被当作格式语法。

`setLogFmt` 设置整行布局，默认是 `[{0}][{1}] {2}`：

| 占位符 | 内容 |
| --- | --- |
| `{0}` | 时间 |
| `{1}` | 等级名称 |
| `{2}` | 格式化后的正文 |

该函数成功时返回 `True`；无效布局返回 `False`，保留旧布局。布局设置后由 Logger 保存，调用者不必维持原字符串的生命周期。

## 使用边界

目前没有文件输出、编辑器订阅或线程安全保证。无效的消息格式可能抛出 `std::format_error`；布局设置的失败返回值不等同于消息提交的错误处理。

`NAMICA_CORE_ASSERT(condition)` 与 `NAMICA_APP_ASSERT(condition)` 用于开发诊断。调用方定义 `NAMICA_ENABLE_ASSERTS` 时，条件失败会输出 Error 并在 Windows 触发调试中断；关闭时不求值 condition。调用时像普通语句一样追加分号；它们不能代替必须执行的错误处理。

当前按 UTF-8 编译文本字面量，但这不会自动设置终端的解码方式；Windows 控制台代码页与 UTF-8 不匹配时，中文仍可能乱码。当前没有终端编码转换或能力检测。输出带 ANSI 颜色，其显示取决于终端支持；重定向得到的文本可能包含颜色控制码。

公开接口：[Log.h](../../runtime/include/namica/core/Log.h) · [LogConfig.h](../../runtime/include/namica/core/LogConfig.h) · [Logger.h](../../runtime/include/namica/core/Logger.h)。返回[用户指南](README.md)。
