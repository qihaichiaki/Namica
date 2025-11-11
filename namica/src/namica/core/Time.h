#pragma once

#include "namica/core/Base.h"
#include <string>

namespace Namica
{

namespace TimeUtils
{

enum class TimeFmtOption
{
    YMD_HMS,  // 年-月-日 时:分:秒
    HMS       // 时:分:秒
};

/**
 * @brief 根据_timeFmt格式化输出本地时间字符串
 *
 * @param _timeFmt 默认格式为: "年-月-日 时:分:秒"
 */
NAMICA_API std::string localTime(TimeFmtOption _timeFmtOpt = TimeFmtOption::YMD_HMS);

}  // namespace TimeUtils

}  // namespace Namica