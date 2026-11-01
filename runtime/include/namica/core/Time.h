#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"

namespace namica
{

// 年-月-日
#define NAMICA_TIME_YMD "{:%Y-%m-%d}"
// 时:分:秒
#define NAMICA_TIME_HMS "{:%H:%M:%S}"
// 年-月-日 时:分:秒
#define NAMICA_TIME_YMD_HMS "{:%Y-%m-%d %H:%M:%S}"

/**
 * @brief 获取当前时区的时间字符串
 *
 * @param _fmt 格式化输出字, %Y年 %m月 %d日 %H小时 %M分钟 %S秒, 需要包含{}, 内部:开头
 * @return NAMICA_API
 */
NAMICA_API String getCurrentTime(String const& _fmt = NAMICA_TIME_HMS);

}  // namespace namica