#include "namica/log/Logger.h"

namespace namica
{

Logger::Logger(String const& _name) : m_name{_name}
{
}

Logger::~Logger()
{
}

SharedPtr<Logger> Logger::create(String const& _name)
{
    return SharedPtr<Logger>{new Logger{_name}};
}

Bool Logger::addSink(SharedPtr<LogSink> _sink)
{
    if (_sink == nullptr)
    {
        return False;
    }

    for (auto const& sink : this->m_logSinks)
    {
        if (sink == _sink)
        {
            // 拒绝重复添加
            return False;
        }
    }

    this->m_logSinks.emplace_back(_sink);

    return True;
}

Bool Logger::removeSink(SharedPtr<LogSink> _sink)
{
    auto it{this->m_logSinks.begin()};
    while (it != this->m_logSinks.end())
    {
        if (*it == _sink)
        {
            this->m_logSinks.erase(it);
            return True;
        }
        it++;
    }

    return False;
}

void Logger::flush()
{
    for (auto& sink : this->m_logSinks)
    {
        sink->flush();
    }
}

}  // namespace namica