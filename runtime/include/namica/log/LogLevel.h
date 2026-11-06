#pragma once

#include "namica/core/Types.h"

namespace namica
{

enum class LogLevel
{
    Trace,     // 高频,非常细的执行过程
    Debug,     // 用于诊断的内部信息
    Info,      // 正常运行中的重要事件
    Warning,   // 出现异常，但仍能继续
    Error,     // 某项操作失败
    Critical,  // 严重故障,正常运行受到破坏
};

inline String const levelStrs[]{"Trace", "Debug", "Info", "Warning", "Error", "Critical"};

inline String const& getLogLevelStr(LogLevel _logLevel) noexcept
{
    return levelStrs[static_cast<UInt8>(_logLevel)];
}

}  // namespace namica