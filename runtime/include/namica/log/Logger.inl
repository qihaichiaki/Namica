#pragma once

#include "namica/log/Logger.h"
#include <format>

namespace namica
{

template <typename... Args>
void Logger::log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args)
{
    auto msg{std::vformat(_msgFmt, std::make_format_args(_args...))};
    // 日志记录
    this->m_logRecords.emplace_back(SystemTimePoint{}, _logLevel, "来源", msg);
    this->println(_logLevel, msg);
}

}  // namespace namica