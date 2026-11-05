#include "namica/log/sink/ConsoleSink.h"

namespace namica
{

// 颜色字符映射
// 默认 蓝绿 绿色 黄色 红色 红色
static char const* levelPreColor[]{"", "\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[31m"};
static char const* levelPostColor[]{"", "\033[0m", "\033[0m", "\033[0m", "\033[0m", "\033[0m"};

SharedPtr<ConsoleSink> ConsoleSink::create(ConsoleSinkConfig const& _config)
{
    return SharedPtr<ConsoleSink>{new ConsoleSink{_config}};
}

}  // namespace namica