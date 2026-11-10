#pragma once

#include <namica/core/Types.h>

#include <iostream>
#include <sstream>

namespace namica::test::log
{

// 在日志测试作用域内借用指定流和缓冲区，将写入导向可观察或可模拟失败的目标。
// 流与缓冲区都必须比本对象存活更久；同一进程内不要并发重定向或使用同一个目标流。
// 退出作用域时恢复原缓冲区、错误状态与异常掩码，不关闭借用的流或缓冲区。
class ScopedStreamRedirect final
{
public:
    ScopedStreamRedirect(std::ostream& _stream, std::streambuf& _buffer);
    ~ScopedStreamRedirect() noexcept;

    ScopedStreamRedirect(ScopedStreamRedirect const&) = delete;
    ScopedStreamRedirect(ScopedStreamRedirect&&) = delete;
    ScopedStreamRedirect& operator=(ScopedStreamRedirect const&) = delete;
    ScopedStreamRedirect& operator=(ScopedStreamRedirect&&) = delete;

private:
    std::ostream& m_stream;
    std::streambuf* m_previous;
    std::ios::iostate m_previousState;
    std::ios::iostate m_previousExceptions;
};

// 为普通日志断言持有字符串缓冲区，默认捕获 std::cout，也可显式选择 std::cerr。
// 本工具只截获对应 C++ ostream 的写入，不接管操作系统标准句柄或 C 的 printf 输出；
// 使用与 ScopedStreamRedirect 相同的借用和并发前提，析构时先解除重定向再释放缓冲区。
class ScopedLogCapture final
{
public:
    explicit ScopedLogCapture(std::ostream& _stream = std::cout);
    ~ScopedLogCapture() noexcept;

    ScopedLogCapture(ScopedLogCapture const&) = delete;
    ScopedLogCapture(ScopedLogCapture&&) = delete;
    ScopedLogCapture& operator=(ScopedLogCapture const&) = delete;
    ScopedLogCapture& operator=(ScopedLogCapture&&) = delete;

    // 返回当前已捕获文本的副本，不清空缓冲区，也不额外触发目标流刷新。
    [[nodiscard]] namica::String text() const;

private:
    // 成员按声明逆序析构，保证 redirect 恢复原流时捕获缓冲区仍然存在。
    std::ostringstream m_buffer{};
    ScopedStreamRedirect m_redirect;
};

}  // namespace namica::test::log
