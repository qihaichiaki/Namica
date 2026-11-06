#pragma once

#include "namica/core/Types.h"
#include "namica/log/sink/LogSinkConfig.h"
#include "namica/core/TimePoint.h"

namespace namica
{

#define NAMICA_DEFAULT_LOG_FMT "[{0}][{1}] {2}"

enum class ConsoleTarget
{
    Stdout,
    Stderr,
};

enum class ConsoleColor
{
    Default,
    Cyan,
    Green,
    Yellow,
    Red
};

struct ConsoleColorTheme
{
    ConsoleColor traceColor{ConsoleColor::Default};
    ConsoleColor debugColor{ConsoleColor::Green};
    ConsoleColor infoColor{ConsoleColor::Cyan};
    ConsoleColor warringColor{ConsoleColor::Yellow};
    ConsoleColor errorColor{ConsoleColor::Red};
    ConsoleColor criticalColor{ConsoleColor::Red};
};

enum class ConsoleColorMode
{
    Auto,    // 检测输出目标，适合显示颜色时才启用
    Always,  // 强制输出颜色表示
    Never,   // 不添加颜色显示
};

struct ConsoleSinkConfig : LogSinkConfig
{
    ConsoleTarget target{ConsoleTarget::Stdout};

    // 日志输出格式(使用fmt格式, 其中0表示日期的输出位置, 1表示level级别的输出位置,
    // 2表示格式化文本msg的输出位置)
    String logFmt{NAMICA_DEFAULT_LOG_FMT};
    String dateFmt{NAMICA_TIME_HMS};
    ConsoleColorTheme colorTheme{};
    ConsoleColorMode colorMode{ConsoleColorMode::Auto};
};

}  // namespace namica