#pragma once

#include "namica/core/Logger.h"
#include <format>

namespace namica
{

template <typename... Args>
void Logger::log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args)
{
    auto msg{std::vformat(_msgFmt, std::make_format_args(std::forward<Args>(_args)...))};
    this->println(_logLevel, msg);
}

}  // namespace namica