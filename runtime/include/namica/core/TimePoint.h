#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"

#include <chrono>

namespace namica
{

// 年-月-日
#define NAMICA_TIME_YMD "{:%Y-%m-%d}"
// 时:分:秒
#define NAMICA_TIME_HMS "{:%H:%M:%S}"
// 年-月-日 时:分:秒
#define NAMICA_TIME_YMD_HMS "{:%Y-%m-%d %H:%M:%S}"

// 系统时间点, 方便看当前系统的日历时间
class SystemTimePoint
{
public:
    NAMICA_API SystemTimePoint();
    NAMICA_API ~SystemTimePoint();

    /**
     * @brief 获取当前时区的时间字符串
     *
     * @param _fmt 格式化输出, %Y年 %m月 %d日 %H小时 %M分钟 %S秒, 需要包含{}, 内部:开头
     * @return NAMICA_API
     */
    NAMICA_API String getStr(String const& _fmt = NAMICA_TIME_HMS) const;

private:
    std::chrono::system_clock::time_point m_timePoint;
};

}  // namespace namica