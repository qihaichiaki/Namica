#pragma once

#include "namica/core/Logger.h"
#include <format>

namespace namica
{

template <typename... Args>
void Logger::log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args)
{
    if (_logLevel >= this->m_minLogLevel)
    {
        auto msg{std::vformat(_msgFmt, std::make_format_args(_args...))};
        this->println(_logLevel, msg);
    }
}

}  // namespace namica