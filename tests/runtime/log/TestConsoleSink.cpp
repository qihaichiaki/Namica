#include "runtime/utils/TestUtils.h"
#include "runtime/log/utils/StreamTestUtils.h"

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

using namica::test::log::ScopedLogCapture;
using namica::test::log::ScopedStreamRedirect;

// 保留写入文本并记录同步次数，供刷新测试观察行为；不依赖真实终端是否立即显示。
// 这些 buffer 只有本文件使用，保留局部即可，不提升到模块共用 utils。
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

// 批量写入返回短写，单字符写入也返回 EOF，保证不同标准库的输出路径都能触发失败。
// 只替换 C++ 流缓冲区，不关闭真实 stdout/stderr，也不依赖磁盘或终端故障。
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

// 正常接收字符，只让 sync 失败，从而将“写入数据失败”和“提交缓冲失败”分开验证。
class RejectingFlushBuffer final : public std::stringbuf
{
protected:
    int sync() override
    {
        return -1;
    }
};

// 默认只输出正文，让各用例按需加入其他字段；显式禁用颜色以排除终端探测和 ANSI 配色。
// 捕获的是窄字符标准流中的文本，不据此声称真实控制台的中文显示或颜色适配已验证。
namica::ConsoleSinkConfig makeConsoleConfig()
{
    namica::ConsoleSinkConfig config{};
    config.logFmt = "{3}";
    config.colorMode = namica::ConsoleColorMode::Never;
    return config;
}

}  // namespace

// 将来源、等级、时间和正文按非默认顺序排布，检查四个占位符是否取自当前 LogRecord。
// 预期时间使用同一条记录的时间点生成，避免跨秒执行造成不稳定；输出还应追加一个换行。
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

// 逐个设置合法布局后写入同一正文，验证 TextSink 的标准格式化语义能通过控制台端输出。
// 即使布局不包含字段或为空，也应接受配置，并保留 ConsoleSink 自行追加行结束符的行为。
TEST_F(NamicaRuntimeTest, ConsoleSinkAcceptsValidLayouts)
{
    struct LayoutCase
    {
        namica::String layout;
        namica::String expected;
    };
    // 数据依次覆盖：只保留正文、重排并重复字段、花括号转义、字符串宽度/精度、
    // 固定文本及空布局。预期值直接列出，避免用同一格式化实现计算后再与自己比较。
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

// 每个非法布局分别用于构造和运行期更新：构造抛格式异常，setter 返回 False 且不抛异常。
// 拒绝更新后继续通过 Logger 写入，既检查旧布局仍生效，也检查原等级阈值未被修改。
TEST_F(NamicaRuntimeTest, ConsoleSinkRejectsInvalidLayoutsAndPreservesConfiguration)
{
    // 覆盖不匹配括号（含合法前缀后接错误）、越界字段、对字符串使用数字格式、
    // 混用自动/显式字段编号，以及拿字符串字段充当动态宽度等不同失效原因。
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

// 先使用 first 布局并拒绝一次非法更新，再切换到 next 布局并再次拒绝非法更新。
// 两次输出都应保留各自最近一次成功设置，防止失败后回退到初始配置或损坏后续更新能力。
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

// 用非法日期格式分别构造和更新 sink，验证创建时抛异常、运行期返回 False 的不同契约。
// 更新失败后仍按原年月日格式输出记录时间，不能留下半更新状态或丢失正文。
TEST_F(NamicaRuntimeTest, ConsoleSinkRejectsInvalidDateFormatsAndPreservesConfiguration)
{
    // 日期格式只有一个时间参数：孤立括号、越界编号和不完整的百分号指令均应拒绝。
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

// 先拒绝非法日期格式，再设置合法年月日格式，检查一次失败不会永久阻止后续配置。
// 使用记录自身的日期构造预期值，只验证新日期格式和正文的组合，不依赖运行当天的固定值。
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

// Config 只提供启动参数：创建后修改布局、日期、等级与目标流，并销毁原配置。
// sink 仍应使用创建时的值写入 stdout，stderr 保持为空，避免误保存 Config 引用。
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

// 将局部布局和日期字符串传给 setter，随后改写并销毁它们，再执行日志输出。
// 输出应保留 setter 接收时的文本，验证运行期配置同样由 sink 持有，不借用调用方字符串。
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

// 同时捕获 stdout/stderr，分别通过两个目标不同的 sink 写入不同标记。
// 每个捕获结果只能包含对应标记和追加换行，防止目标选择失效或一次写入重复派发到两端。
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

// 通过 Logger 分别提交空正文和包含中文、参数、转义花括号、内部换行的正文。
// 前端完成参数格式化后，sink 只组合等级与正文；正文中的花括号不能被再次解析，
// 内部换行按原样保留，空正文也应获得 ConsoleSink 追加的行结束符。
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

// 两个 sink 借用同一 stdout，但使用不同的布局和等级；交错写入后先销毁其中一个。
// 预期每个端独立过滤和格式化，剩余端仍能继续输出，验证销毁借用者不会关闭共享标准流。
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

// 用可计数缓冲区观察 write 后的刷新，再直接调用 flush，确认两条公开路径都会同步。
// 只要求同步次数增加，不固定标准流可能附带的刷新次数；刷新后文本内容仍应保留且不重复。
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

// 将 stdout 临时接到拒绝写入的缓冲区，检查 ConsoleSink::write 能以 False 反馈流失败。
// 先退出重定向作用域、恢复 stdout，再断言结果，避免失败状态影响 GoogleTest 后续输出。
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

// 缓冲区可正常接收字符但拒绝同步，分别检查显式 flush 和 write 内部刷新均返回 False。
// 两次操作之间清除流状态，确保第二次不是仅沿用上次的失败标记，而是实际再次执行刷新。
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
