#include "runtime/log/utils/StreamTestUtils.h"

#include <namica/core/Types.h>

namespace namica::test::log
{

ScopedStreamRedirect::ScopedStreamRedirect(std::ostream& _stream, std::streambuf& _buffer)
    : m_stream{_stream},
      m_previous{_stream.rdbuf()},
      m_previousState{_stream.rdstate()},
      m_previousExceptions{_stream.exceptions()}
{
    // rdbuf 的替换同时清除当前错误状态，让捕获从可用流状态开始；原状态已保存供退出恢复。
    this->m_stream.rdbuf(&_buffer);
}

ScopedStreamRedirect::~ScopedStreamRedirect() noexcept
{
    // 失败用例可能留下 badbit/failbit；先临时关闭流异常，再恢复缓冲区和原错误状态，
    // 避免恢复中途抛异常而把标准流继续留在测试缓冲区上，污染后续用例和 GoogleTest 输出。
    this->m_stream.exceptions(std::ios::goodbit);
    this->m_stream.rdbuf(this->m_previous);
    this->m_stream.clear(this->m_previousState);
    try
    {
        this->m_stream.exceptions(this->m_previousExceptions);
    }
    catch (std::ios_base::failure const&)
    {
        // 原状态若已触发原掩码，恢复掩码会报告 ios_base::failure，但状态和掩码已设置完成。
        // 只在这个恢复边界吸收异常，保证清理不从析构逃逸，不改变被测输出操作的失败结果。
    }
}

ScopedLogCapture::ScopedLogCapture(std::ostream& _stream)
    : m_redirect{_stream, *this->m_buffer.rdbuf()}
{
}

ScopedLogCapture::~ScopedLogCapture() noexcept = default;

namica::String ScopedLogCapture::text() const
{
    return this->m_buffer.str();
}

}  // namespace namica::test::log
