#pragma once

#include "namica/log/Logger.h"
#include "namica/log/LogRecord.h"

#include <format>

namespace namica
{

template <typename... Args>
void Logger::log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args)
{
    Bool isInit{True};

    LogRecord logRecord{};
    for (auto& sink : this->m_logSinks)
    {
        if (_logLevel >= sink->getLogMinLevel())
        {
            if (isInit)
            {
                logRecord.logLevel = _logLevel;
                logRecord.source = this->m_name;
                try
                {
                    logRecord.message = std::vformat(_msgFmt, std::make_format_args(_args...));
                }
                catch (...)
                {
                    break;
                }

                isInit = False;
            }

            sink->write(logRecord);
        }
    }
}

}  // namespace namica