#pragma once

#include "namica/core/Types.h"
#include "namica/log/sink/LogSinkConfig.h"

namespace namica
{

enum class ConsoleColor
{
    Default,
    Blue,
    Green,
    Yellow,
    Red
};

struct LevelColorConfig
{
    ConsoleColor traceColor{ConsoleColor::Default};
    ConsoleColor debugColor{ConsoleColor::Green};
    ConsoleColor infoColor{ConsoleColor::Blue};
    ConsoleColor warringColor{ConsoleColor::Yellow};
    ConsoleColor errorColor{ConsoleColor::Red};
    ConsoleColor criticalColor{ConsoleColor::Red};
};

struct LogColorConfig
{
    ConsoleColor dataColor{ConsoleColor::Default};
    LevelColorConfig levelColor{};
    ConsoleColor msgColor{ConsoleColor::Default};
};

struct ConsoleSinkConfig : public LogSinkConfig
{
    // 日志输出格式(使用fmt格式, 其中0表示日期的输出位置, 1表示level级别的输出位置,
    // 2表示格式化文本msg的输出位置)
    String logFmt{"[{0}][{1}] {2}"};
    LogColorConfig logColor{};
};

}  // namespace namica