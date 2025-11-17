#pragma once

#include "namica/core/Base.h"
#include <string>

namespace Namica
{

class NAMICA_API Timestep
{
public:
    Timestep(float _time = 0.0f);

    /// @brief 注意默认步长为s数进行使用
    operator float() const noexcept;
    /// @brief 获得当前时间步长的s数
    float getSeconds() const noexcept;
    /// @brief 获得当前时间步长的ms数
    float getMilliseconds() const noexcept;

private:
    float m_time;
};

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