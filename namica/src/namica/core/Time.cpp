#include "namica/core/Time.h"
#include <chrono>
#include <format>

namespace Namica
{

namespace TimeUtils
{

std::string localTime(TimeFmtOption _timeFmtOpt)
{
    using namespace std::chrono;

    // 获取当前系统时间
    auto now = system_clock::now();
    // 转换为本地时间（std::chrono::zoned_time）
    auto local = floor<seconds>(current_zone()->to_local(now));

    // 格式化输出
    if (_timeFmtOpt == TimeFmtOption::HMS)
    {
        return std::format("{:%H:%M:%S}", local);
    }
    else
    {
        return std::format("{:%Y-%m-%d %H:%M:%S}", local);
    }
}

}  // namespace TimeUtils
}  // namespace Namica