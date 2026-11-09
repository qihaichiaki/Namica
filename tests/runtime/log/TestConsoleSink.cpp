#include "runtime/utils/TestUtils.h"

#include <namica/core/Types.h>
#include <namica/log/Logger.h>
#include <namica/log/LogRecord.h>
#include <namica/log/sink/ConsoleSink.h>
#include <namica/log/sink/ConsoleSinkConfig.h>

#include <array>
#include <format>
#include <iostream>
#include <sstream>

namespace
{

// 失败状态也必须恢复，避免一个输出失败用例污染后续测试和 GoogleTest 输出。
class ScopedStreamRedirect final
{
public:
    ScopedStreamRedirect(std::ostream& _stream, std::streambuf& _buffer)
        : m_stream{_stream}, m_previous{_stream.rdbuf()}, m_previousState{_stream.rdstate()}
    {
        this->m_stream.rdbuf(&_buffer);
    }

    ~ScopedStreamRedirect() noexcept
    {
        this->m_stream.rdbuf(this->m_previous);
        this->m_stream.clear(this->m_previousState);
    }

    ScopedStreamRedirect(ScopedStreamRedirect const&) = delete;
    ScopedStreamRedirect& operator=(ScopedStreamRedirect const&) = delete;

private:
    std::ostream& m_stream;
    std::streambuf* m_previous;
    std::ios::iostate m_previousState;
};

class ScopedLogCapture final
{
public:
    explicit ScopedLogCapture(std::ostream& _stream = std::cout)
        : m_redirect{_stream, *this->m_buffer.rdbuf()}
    {
    }

    [[nodiscard]] namica::String text() const
    {
        return this->m_buffer.str();
    }

private:
    std::ostringstream m_buffer{};
    ScopedStreamRedirect m_redirect;
};

class FlushCountingBuffer final : public std::stringbuf
{
public:
    [[nodiscard]] namica::Int flushCalls() const noexcept
    {
        return this->m_flushCalls;
    }

protected:
    int sync() override
    {
        ++this->m_flushCalls;
        return this->std::stringbuf::sync();
    }

private:
    namica::Int m_flushCalls{0};
};

class RejectingWriteBuffer final : public std::streambuf
{
protected:
    std::streamsize xsputn(namica::Char const*, std::streamsize) override
    {
        return 0;
    }

    int_type overflow(int_type) override
    {
        return traits_type::eof();
    }
};

class RejectingFlushBuffer final : public std::stringbuf
{
protected:
    int sync() override
    {
        return -1;
    }
};

// 文本测试显式禁用颜色，不依赖宿主终端能力，也不固定 ANSI 配色实现。
namica::ConsoleSinkConfig makeConsoleConfig()
{
    namica::ConsoleSinkConfig config{};
    config.logFmt = "{3}";
    config.colorMode = namica::ConsoleColorMode::Never;
    return config;
}

}  // namespace

TEST_F(NamicaRuntimeTest, ConsoleSinkUsesRecordLayoutFields)
{
    auto config{makeConsoleConfig()};
    config.logFmt = "{2}|{1}|{0}|{3}";
    config.dateFmt = NAMICA_TIME_YMD_HMS;
    auto sink{namica::ConsoleSink::create(config)};
    namica::LogRecord record{};
    record.logLevel = namica::LogLevel::Warning;
    record.source = "Runtime";
    record.message = "payload";
    auto const expected{namica::String{"Runtime|Warning|"} +
                        record.timePoint.getStr(config.dateFmt) + "|payload\n"};
    ScopedLogCapture capture{};

    EXPECT_TRUE(sink->write(record));

    EXPECT_EQ(capture.text(), expected);
}

// 省略、重排、重复字段以及字符串格式说明符都应使用标准格式化语义。
TEST_F(NamicaRuntimeTest, ConsoleSinkAcceptsValidLayouts)
{
    struct LayoutCase
    {
        namica::String layout;
        namica::String expected;
    };
    std::array const cases{LayoutCase{"{3}", "payload\n"},
                           LayoutCase{"{3}|{1}|{3}", "payload|Info|payload\n"},
                           LayoutCase{"{{{3}}}", "{payload}\n"},
                           LayoutCase{"{1:>8}|{3:.3}", "    Info|pay\n"},
                           LayoutCase{"fixed text", "fixed text\n"},
                           LayoutCase{"", "\n"}};

    for (auto const& layoutCase : cases)
    {
        SCOPED_TRACE(layoutCase.layout);
        auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
        ASSERT_TRUE(sink->setLogFmt(layoutCase.layout));
        namica::LogRecord record{};
        record.message = "payload";
        ScopedLogCapture capture{};

        EXPECT_TRUE(sink->write(record));

        EXPECT_EQ(capture.text(), layoutCase.expected);
    }
}

// 非法布局应在构造时暴露；运行期拒绝后仍使用最近一次有效布局和等级。
TEST_F(NamicaRuntimeTest, ConsoleSinkRejectsInvalidLayoutsAndPreservesConfiguration)
{
    std::array const invalidLayouts{namica::String{"{"},
                                    namica::String{"}"},
                                    namica::String{"{2} {"},
                                    namica::String{"{99}"},
                                    namica::String{"{0:d}"},
                                    namica::String{"{1:d}"},
                                    namica::String{"{3:d}"},
                                    namica::String{"{} {1}"},
                                    namica::String{"{0} {}"},
                                    namica::String{"{3:{1}}"}};

    for (auto const& layout : invalidLayouts)
    {
        SCOPED_TRACE(layout);
        auto invalidConfig{makeConsoleConfig()};
        invalidConfig.logFmt = layout;
        EXPECT_THROW(namica::ConsoleSink::create(invalidConfig), std::format_error);

        auto config{makeConsoleConfig()};
        config.logFmt = "saved:{1}|{3}";
        config.minLevel = namica::LogLevel::Warning;
        auto sink{namica::ConsoleSink::create(config)};
        auto logger{namica::Logger::create()};
        ASSERT_TRUE(logger->addSink(sink));
        namica::Bool accepted{namica::True};
        EXPECT_NO_THROW(accepted = sink->setLogFmt(layout));
        EXPECT_FALSE(accepted);
        ScopedLogCapture capture{};

        logger->log(namica::LogLevel::Info, "hidden");
        logger->log(namica::LogLevel::Error, "payload");

        EXPECT_EQ(capture.text(), "saved:Error|payload\n");
        EXPECT_EQ(sink->getLogMinLevel(), namica::LogLevel::Warning);
    }
}

TEST_F(NamicaRuntimeTest, ConsoleSinkChangesLayoutAfterRejectedUpdate)
{
    auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
    namica::LogRecord record{};
    record.message = "payload";
    ASSERT_TRUE(sink->setLogFmt("first:{3}"));
    ASSERT_FALSE(sink->setLogFmt("{"));
    {
        ScopedLogCapture capture{};
        EXPECT_TRUE(sink->write(record));
        EXPECT_EQ(capture.text(), "first:payload\n");
    }

    ASSERT_TRUE(sink->setLogFmt("next:{1}|{3}"));
    ASSERT_FALSE(sink->setLogFmt("}"));
    {
        ScopedLogCapture capture{};
        EXPECT_TRUE(sink->write(record));
        EXPECT_EQ(capture.text(), "next:Info|payload\n");
    }
}

TEST_F(NamicaRuntimeTest, ConsoleSinkRejectsInvalidDateFormatsAndPreservesConfiguration)
{
    std::array const invalidFormats{
        namica::String{"{"}, namica::String{"}"}, namica::String{"{1}"}, namica::String{"{:%}"}};
    for (auto const& dateFormat : invalidFormats)
    {
        SCOPED_TRACE(dateFormat);
        auto invalidConfig{makeConsoleConfig()};
        invalidConfig.dateFmt = dateFormat;
        EXPECT_THROW(namica::ConsoleSink::create(invalidConfig), std::format_error);

        auto config{makeConsoleConfig()};
        config.logFmt = "{0}|{3}";
        config.dateFmt = NAMICA_TIME_YMD;
        auto sink{namica::ConsoleSink::create(config)};
        namica::Bool accepted{namica::True};
        EXPECT_NO_THROW(accepted = sink->setDateFmt(dateFormat));
        EXPECT_FALSE(accepted);
        namica::LogRecord record{};
        record.message = "payload";
        ScopedLogCapture capture{};

        EXPECT_TRUE(sink->write(record));

        EXPECT_EQ(capture.text(), record.timePoint.getStr(NAMICA_TIME_YMD) + "|payload\n");
    }
}

TEST_F(NamicaRuntimeTest, ConsoleSinkChangesDateFormatAfterRejectedUpdate)
{
    auto config{makeConsoleConfig()};
    config.logFmt = "{0}|{3}";
    auto sink{namica::ConsoleSink::create(config)};
    ASSERT_FALSE(sink->setDateFmt("{"));
    ASSERT_TRUE(sink->setDateFmt(NAMICA_TIME_YMD));
    namica::LogRecord record{};
    record.message = "payload";
    ScopedLogCapture capture{};

    EXPECT_TRUE(sink->write(record));

    EXPECT_EQ(capture.text(), record.timePoint.getStr(NAMICA_TIME_YMD) + "|payload\n");
}

// Config 是启动参数；源配置的修改、销毁均不能改变已创建的输出端。
TEST_F(NamicaRuntimeTest, ConsoleSinkOwnsStartupConfiguration)
{
    namica::SharedPtr<namica::ConsoleSink> sink{};
    {
        auto config{makeConsoleConfig()};
        config.logFmt = "owned:{0}|{3}";
        config.dateFmt = NAMICA_TIME_YMD;
        config.minLevel = namica::LogLevel::Warning;
        sink = namica::ConsoleSink::create(config);
        config.logFmt = "changed:{3}";
        config.dateFmt = NAMICA_TIME_HMS;
        config.minLevel = namica::LogLevel::Critical;
        config.target = namica::ConsoleTarget::Stderr;
    }
    namica::LogRecord record{};
    record.logLevel = namica::LogLevel::Warning;
    record.message = "payload";
    ScopedLogCapture output{std::cout};
    ScopedLogCapture errors{std::cerr};

    EXPECT_TRUE(sink->write(record));

    EXPECT_EQ(output.text(), "owned:" + record.timePoint.getStr(NAMICA_TIME_YMD) + "|payload\n");
    EXPECT_TRUE(errors.text().empty());
    EXPECT_EQ(sink->getLogMinLevel(), namica::LogLevel::Warning);
}

TEST_F(NamicaRuntimeTest, ConsoleSinkOwnsRuntimeFormatStrings)
{
    auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
    {
        namica::String layout{"owned:{0}|{3}"};
        namica::String dateFormat{NAMICA_TIME_YMD};
        ASSERT_TRUE(sink->setLogFmt(layout));
        ASSERT_TRUE(sink->setDateFmt(dateFormat));
        layout = "changed:{3}";
        dateFormat = NAMICA_TIME_HMS;
    }
    namica::LogRecord record{};
    record.message = "payload";
    ScopedLogCapture capture{};

    EXPECT_TRUE(sink->write(record));

    EXPECT_EQ(capture.text(), "owned:" + record.timePoint.getStr(NAMICA_TIME_YMD) + "|payload\n");
}

TEST_F(NamicaRuntimeTest, ConsoleSinkTargetsSelectedStandardStream)
{
    auto outputConfig{makeConsoleConfig()};
    auto errorConfig{makeConsoleConfig()};
    errorConfig.target = namica::ConsoleTarget::Stderr;
    auto outputSink{namica::ConsoleSink::create(outputConfig)};
    auto errorSink{namica::ConsoleSink::create(errorConfig)};
    namica::LogRecord record{};
    ScopedLogCapture output{std::cout};
    ScopedLogCapture errors{std::cerr};

    record.message = "stdout";
    EXPECT_TRUE(outputSink->write(record));
    record.message = "stderr";
    EXPECT_TRUE(errorSink->write(record));

    EXPECT_EQ(output.text(), "stdout\n");
    EXPECT_EQ(errors.text(), "stderr\n");
}

// 布局独立于正文：正文中的中文、花括号和内部换行不得再次作为格式串解析。
TEST_F(NamicaRuntimeTest, ConsoleSinkPreservesMessageAndAppendsNewline)
{
    auto config{makeConsoleConfig()};
    config.logFmt = "{1}|{3}";
    auto sink{namica::ConsoleSink::create(config)};
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->addSink(sink));
    ScopedLogCapture capture{};

    logger->log(namica::LogLevel::Info, "");
    logger->log(namica::LogLevel::Warning, "资源：{} {{ready}}\nnext", namica::String{"贪吃蛇"});

    EXPECT_EQ(capture.text(), "Info|\nWarning|资源：贪吃蛇 {ready}\nnext\n");
}

TEST_F(NamicaRuntimeTest, ConsoleSinkKeepsInstancesIndependent)
{
    auto secondConfig{makeConsoleConfig()};
    secondConfig.logFmt = "second:{3}";
    secondConfig.minLevel = namica::LogLevel::Debug;
    auto secondSink{namica::ConsoleSink::create(secondConfig)};
    auto second{namica::Logger::create()};
    ASSERT_TRUE(second->addSink(secondSink));
    ScopedLogCapture capture{};
    {
        auto firstConfig{makeConsoleConfig()};
        firstConfig.logFmt = "first:{3}";
        firstConfig.minLevel = namica::LogLevel::Error;
        auto firstSink{namica::ConsoleSink::create(firstConfig)};
        auto first{namica::Logger::create()};
        ASSERT_TRUE(first->addSink(firstSink));
        first->log(namica::LogLevel::Info, "hidden");
        second->log(namica::LogLevel::Debug, "debug");
        first->log(namica::LogLevel::Error, "error");
    }
    second->log(namica::LogLevel::Info, "still-alive");

    EXPECT_EQ(capture.text(), "second:debug\nfirst:error\nsecond:still-alive\n");
}

TEST_F(NamicaRuntimeTest, ConsoleSinkFlushesWritesAndSupportsExplicitFlush)
{
    auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
    namica::LogRecord record{};
    record.message = "payload";
    FlushCountingBuffer buffer{};
    ScopedStreamRedirect redirect{std::cout, buffer};

    EXPECT_TRUE(sink->write(record));
    auto const flushesAfterWrite{buffer.flushCalls()};
    EXPECT_GT(flushesAfterWrite, 0);
    EXPECT_EQ(buffer.str(), "payload\n");
    EXPECT_TRUE(sink->flush());
    EXPECT_GT(buffer.flushCalls(), flushesAfterWrite);
    EXPECT_EQ(buffer.str(), "payload\n");
}

// 用短写模拟 I/O 失败，不需要关闭真实标准句柄或依赖磁盘、终端环境。
TEST_F(NamicaRuntimeTest, ConsoleSinkReturnsFalseOnWriteFailure)
{
    auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
    namica::LogRecord record{};
    record.message = "payload";
    namica::Bool succeeded{namica::True};
    {
        RejectingWriteBuffer buffer{};
        ScopedStreamRedirect redirect{std::cout, buffer};
        succeeded = sink->write(record);
    }

    EXPECT_FALSE(succeeded);
}

TEST_F(NamicaRuntimeTest, ConsoleSinkReturnsFalseOnFlushFailure)
{
    auto sink{namica::ConsoleSink::create(makeConsoleConfig())};
    namica::LogRecord record{};
    record.message = "payload";
    namica::Bool flushSucceeded{namica::True};
    namica::Bool writeSucceeded{namica::True};
    {
        RejectingFlushBuffer buffer{};
        ScopedStreamRedirect redirect{std::cout, buffer};
        flushSucceeded = sink->flush();
        // 清除上次失败状态，确保第二次确实走过 write 内部的刷新。
        std::cout.clear();
        writeSucceeded = sink->write(record);
    }

    EXPECT_FALSE(flushSucceeded);
    EXPECT_FALSE(writeSucceeded);
}
