# 日志

`Logger` 用于输出带等级的诊断消息，当前采用同步控制台输出。

## 创建和输出

在已链接 `Namica` 的应用中使用：

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

`Critical` 本身不会终止程序。调用日志函数前的参数求值仍然会发生，即使消息随后被过滤。

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

目前没有文件输出、编辑器订阅或线程安全保证，`Log.h` 也尚无统一入口。无效的消息格式可能抛出 `std::format_error`；布局设置的失败返回值不等同于消息提交的错误处理。

控制台输出带 ANSI 颜色，其显示取决于终端支持；重定向得到的文本可能包含颜色控制码。

公开接口：[Logger.h](../../runtime/include/namica/core/Logger.h)。返回[用户指南](README.md)。
