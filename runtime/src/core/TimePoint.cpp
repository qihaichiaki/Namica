#include "namica/core/TimePoint.h"
#include <chrono>
#include <format>

namespace namica
{

SystemTimePoint::SystemTimePoint() : m_timePoint{std::chrono::system_clock::now()}
{
}

SystemTimePoint::~SystemTimePoint()
{
}

String SystemTimePoint::getStr(String const& _fmt)
{
    auto localTime{std::chrono::floor<std::chrono::seconds>(
        std::chrono::current_zone()->to_local(this->m_timePoint))};
    return std::vformat(_fmt, std::make_format_args(localTime));
}

}  // namespace namica