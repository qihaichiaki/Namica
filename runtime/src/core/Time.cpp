#include "namica/core/Time.h"
#include <chrono>
#include <format>

namespace namica
{

String getCurrentTime(String const& _fmt)
{
    auto timePoint{std::chrono::system_clock::now()};
    auto localTime{
        std::chrono::floor<std::chrono::seconds>(std::chrono::current_zone()->to_local(timePoint))};

    return std::vformat(_fmt, std::make_format_args(localTime));
}

}  // namespace namica