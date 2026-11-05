# 基础类型与时间

core 提供通用类型和时间点；日志位于 log 模块，使用说明见[日志指南](Logging.md)。

## 常用类型

包含 `<namica/core/Types.h>` 后，可以使用以下名称。它们是标准 C++ 类型的别名，不改变原有语义。

| 名称 | 含义 |
| --- | --- |
| `Int / UInt` | 32 位有符号 / 无符号整数 |
| `Int8 / Int32 / Int64`、`UInt8 / UInt32 / UInt64` | 指定位宽的整数 |
| `Float / Double` | `float / double` |
| `Bool / Char / UChar` | `bool / char / unsigned char` |
| `String` | `std::string` |
| `UniquePtr<T> / SharedPtr<T> / WeakPtr<T>` | 对应的标准智能指针 |

例如，图片尺寸使用 `namica::Int`，图片字节使用 `namica::UChar`。标准库容器和 `std::filesystem::path` 仍照常使用。

## 获取日期时间文本

`SystemTimePoint` 在构造时保存当前系统时间点，`getStr()` 按系统时区返回该时间点的文本，适合日志时间戳和文本显示：

```cpp
#include <namica/core/TimePoint.h>
#include <namica/core/Types.h>
#include <iostream>

int main()
{
    namica::SystemTimePoint timePoint{};
    namica::String const time{timePoint.getStr()};
    namica::String const dateTime{timePoint.getStr(NAMICA_TIME_YMD_HMS)};

    std::cout << time << '\n' << dateTime << '\n';
    return 0;
}
```

| 格式 | 显示内容 |
| --- | --- |
| 默认 / `NAMICA_TIME_HMS` | `时:分:秒` |
| `NAMICA_TIME_YMD` | `年-月-日` |
| `NAMICA_TIME_YMD_HMS` | `年-月-日 时:分:秒` |

也可以传入日期时间格式串，如 `timePoint.getStr("{:%Y/%m/%d}")`。格式包含外层大括号和冒号。

## 使用边界

同一实例重复调用 getStr 不会重新采集当前时间；其日历时间文本精度为秒，不提供帧间隔、秒表或游戏计时功能。自定义格式错误及系统时区查询失败可能抛出异常，接口没有统一失败返回值。

公开接口：[Types.h](../../runtime/include/namica/core/Types.h) · [TimePoint.h](../../runtime/include/namica/core/TimePoint.h)。返回[用户指南](README.md)。
