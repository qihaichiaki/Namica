#include "runtime/utils/TestUtils.h"

#include <namica/core/Logger.h>
#include <namica/core/Types.h>

#include <array>
#include <format>
#include <iostream>
#include <regex>
#include <sstream>

namespace
{

// 只观察格式化是否发生，不把日志调用前的参数求值误认为格式化开销。
struct LoggerFormatProbe
{
    namica::Int& formatCalls;
};

}  // namespace

template <>
struct std::formatter<LoggerFormatProbe> : std::formatter<namica::Int>
{
    auto format(LoggerFormatProbe const& _value, std::format_context& _context) const
    {
        ++_value.formatCalls;
        return std::formatter<namica::Int>::format(namica::Int{7}, _context);
    }
};

namespace
{

// 测试通过现有控制台出口观察行为；失败或抛异常时也必须恢复 cout。
class ScopedLogCapture final
{
public:
    ScopedLogCapture() : m_previous{std::cout.rdbuf(m_buffer.rdbuf())}
    {
    }

    ~ScopedLogCapture()
    {
        std::cout.rdbuf(m_previous);
    }

    ScopedLogCapture(ScopedLogCapture const&) = delete;
    ScopedLogCapture& operator=(ScopedLogCapture const&) = delete;

    namica::String text() const
    {
        // 颜色是终端呈现策略；这些测试只约束消息与布局，不固定 ANSI 配色。
        static std::regex const ansiColor{"\x1b\\[[0-9;]*m"};
        return std::regex_replace(m_buffer.str(), ansiColor, "");
    }

private:
    std::ostringstream m_buffer;
    std::streambuf* m_previous;
};

constexpr std::array kLogLevels{namica::LogLevel::Trace,
                                namica::LogLevel::Debug,
                                namica::LogLevel::Info,
                                namica::LogLevel::Warning,
                                namica::LogLevel::Error,
                                namica::LogLevel::Critical};

}  // namespace

// 覆盖每个阈值的边界：等于阈值也要输出，低于阈值才被丢弃。
TEST_F(NamicaRuntimeTest, LoggerFiltersAtEveryThreshold)
{
    struct FilterCase
    {
        namica::LogLevel minimum;
        namica::Char const* expected;
    };

    // 显式列出结果，不在测试里重复实现 level >= minimum 的判断。
    constexpr std::array cases{
        FilterCase{namica::LogLevel::Trace, "Trace\nDebug\nInfo\nWarning\nError\nCritical\n"},
        FilterCase{namica::LogLevel::Debug, "Debug\nInfo\nWarning\nError\nCritical\n"},
        FilterCase{namica::LogLevel::Info, "Info\nWarning\nError\nCritical\n"},
        FilterCase{namica::LogLevel::Warning, "Warning\nError\nCritical\n"},
        FilterCase{namica::LogLevel::Error, "Error\nCritical\n"},
        FilterCase{namica::LogLevel::Critical, "Critical\n"}};

    for (auto const& filterCase : cases)
    {
        SCOPED_TRACE(static_cast<namica::Int>(filterCase.minimum));
        auto logger{namica::Logger::create()};
        ASSERT_TRUE(logger->setLogFmt("{1}"));
        logger->setLogMinLevel(filterCase.minimum);
        ScopedLogCapture capture;

        for (auto level : kLogLevels)
        {
            logger->log(level, "你好世界!");
        }

        EXPECT_EQ(capture.text(), filterCase.expected);
    }
}

// 默认配置约定为全等级输出；显式指定布局，避免依赖默认显示样式。
TEST_F(NamicaRuntimeTest, LoggerAcceptsAllLevelsByDefault)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{1}"));
    ScopedLogCapture capture;

    for (auto level : kLogLevels)
    {
        logger->log(level, "message");
    }

    EXPECT_EQ(capture.text(), "Trace\nDebug\nInfo\nWarning\nError\nCritical\n");
}

// 同一实例降低阈值后，应立即接收此前被过滤的等级，无需重新创建 Logger。
TEST_F(NamicaRuntimeTest, LoggerChangesMinimumLevelAtRuntime)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    ScopedLogCapture capture;

    logger->setLogMinLevel(namica::LogLevel::Error);
    logger->log(namica::LogLevel::Warning, "hidden-warning");
    logger->log(namica::LogLevel::Error, "visible-error");
    logger->setLogMinLevel(namica::LogLevel::Debug);
    logger->log(namica::LogLevel::Debug, "visible-debug");
    logger->log(namica::LogLevel::Trace, "hidden-trace");

    EXPECT_EQ(capture.text(), "visible-error\nvisible-debug\n");
}

// 混合验证常量字符串、整数补零和浮点精度；与后面的右值用例共同覆盖参数传递。
TEST_F(NamicaRuntimeTest, LoggerFormatsLvalueArguments)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    namica::String const name{"snake"};
    namica::Int count{42};
    namica::Double duration{3.5};
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, "{0}:{1:04d}:{2:.1f}", name, count, duration);

    EXPECT_EQ(capture.text(), "snake:0042:3.5\n");
}

// 临时整数也是合法日志参数；本用例同时检查调用能编译、输出值正确。
TEST_F(NamicaRuntimeTest, LoggerFormatsIntegerRvalues)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, "value={}", namica::Int{42});

    EXPECT_EQ(capture.text(), "value=42\n");
}

// 临时字符串在同步调用结束前有效，Logger 应能在这段有效期内完成格式化。
TEST_F(NamicaRuntimeTest, LoggerFormatsTemporaryStrings)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, "value={}", namica::String{"temporary"});

    EXPECT_EQ(capture.text(), "value=temporary\n");
}

// 格式串可以在运行中变化，后续调用不能沿用先前的消息格式。
TEST_F(NamicaRuntimeTest, LoggerAcceptsRuntimeMessageFormats)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    namica::String messageFormat{"value={}"};
    namica::Int value{12};
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, messageFormat, value);
    messageFormat = "next={:03d}";
    logger->log(namica::LogLevel::Info, messageFormat, value);

    EXPECT_EQ(capture.text(), "value=12\nnext=012\n");
}

// 中文内容应完整保留；转义产生的大括号不能被外层日志布局再次当作占位符。
TEST_F(NamicaRuntimeTest, LoggerPreservesUtf8AndEscapedBraces)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    namica::String asset{"贪吃蛇"};
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, "资源：{} {{ready}}", asset);

    EXPECT_EQ(capture.text(), "资源：贪吃蛇 {ready}\n");
}

// 空消息仍是一条有效记录，应保留所选布局中的等级和行结束符。
TEST_F(NamicaRuntimeTest, LoggerWritesEmptyMessages)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{1}|{2}"));
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Info, "");

    EXPECT_EQ(capture.text(), "Info|\n");
}

TEST_F(NamicaRuntimeTest, LoggerUsesCustomLayoutFields)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}|{1}|{0}"));
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Warning, "payload");

    // 只检查公开约定的 0/1/2 字段，不固定默认布局或某个真实时间点。
    std::regex const expected{R"(payload\|Warning\|[0-9]{2}:[0-9]{2}:[0-9]{2}\n)"};
    EXPECT_TRUE(std::regex_match(capture.text(), expected)) << capture.text();
}

// 有效布局可以省略、重复字段，也可以使用转义和字符串格式说明符。
TEST_F(NamicaRuntimeTest, LoggerAcceptsValidLayouts)
{
    struct LayoutCase
    {
        namica::String layout;
        namica::String expected;
    };

    std::array const cases{LayoutCase{"{2}", "payload\n"},
                           LayoutCase{"{2}|{1}|{2}", "payload|Info|payload\n"},
                           LayoutCase{"{{{2}}}", "{payload}\n"},
                           LayoutCase{"{1:>8}|{2:.3}", "    Info|pay\n"},
                           LayoutCase{"fixed text", "fixed text\n"},
                           LayoutCase{"", "\n"}};

    for (auto const& layoutCase : cases)
    {
        SCOPED_TRACE(testing::Message() << "layout = \"" << layoutCase.layout << "\"");
        auto logger{namica::Logger::create()};
        ASSERT_TRUE(logger->setLogFmt(layoutCase.layout));
        ScopedLogCapture capture;

        logger->log(namica::LogLevel::Info, "payload");

        EXPECT_EQ(capture.text(), layoutCase.expected);
    }
}

// 配置错误应在设置时返回失败，不能破坏旧布局或推迟到下一条日志才抛错。
TEST_F(NamicaRuntimeTest, LoggerRejectsInvalidLayoutsWithoutChangingState)
{
    std::array const invalidLayouts{namica::String{"{"},
                                    namica::String{"}"},
                                    namica::String{"{2} {"},
                                    namica::String{"{3}"},
                                    namica::String{"{0:d}"},
                                    namica::String{"{1:d}"},
                                    namica::String{"{2:d}"},
                                    namica::String{"{} {1}"},
                                    namica::String{"{0} {}"},
                                    namica::String{"{2:{1}}"}};

    for (auto const& layout : invalidLayouts)
    {
        SCOPED_TRACE(layout);
        auto logger{namica::Logger::create()};
        ASSERT_TRUE(logger->setLogFmt("saved:{1}|{2}"));
        logger->setLogMinLevel(namica::LogLevel::Warning);

        namica::Bool accepted{namica::True};
        EXPECT_NO_THROW(accepted = logger->setLogFmt(layout));
        EXPECT_FALSE(accepted);

        // 通过后续输出验证状态，不依赖私有成员或默认布局；等级过滤也应保持。
        ScopedLogCapture capture;
        EXPECT_NO_THROW(logger->log(namica::LogLevel::Info, "hidden"));
        EXPECT_NO_THROW(logger->log(namica::LogLevel::Error, "payload"));
        EXPECT_EQ(capture.text(), "saved:Error|payload\n");
    }
}

// 一次失败不能令配置入口失效；之后仍能替换布局，并保留最近一次成功的配置。
TEST_F(NamicaRuntimeTest, LoggerChangesLayoutAfterRejectedUpdate)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("first:{2}"));
    ASSERT_FALSE(logger->setLogFmt("{3}"));
    {
        ScopedLogCapture capture;
        logger->log(namica::LogLevel::Info, "before");
        EXPECT_EQ(capture.text(), "first:before\n");
    }

    ASSERT_TRUE(logger->setLogFmt("next:{1}|{2}"));
    {
        ScopedLogCapture capture;
        logger->log(namica::LogLevel::Warning, "changed");
        EXPECT_EQ(capture.text(), "next:Warning|changed\n");
    }

    ASSERT_FALSE(logger->setLogFmt("{"));
    {
        ScopedLogCapture capture;
        logger->log(namica::LogLevel::Error, "still-valid");
        EXPECT_EQ(capture.text(), "next:Error|still-valid\n");
    }
}

// 设置成功后布局由 Logger 保存；调用方修改或销毁源字符串不应改变已生效配置。
TEST_F(NamicaRuntimeTest, LoggerOwnsConfiguredLayout)
{
    auto logger{namica::Logger::create()};
    {
        namica::String layout{"owned:{2}"};
        ASSERT_TRUE(logger->setLogFmt(layout));
        layout.replace(0, 5, "other");

        ScopedLogCapture capture;
        logger->log(namica::LogLevel::Info, "source-changed");
        EXPECT_EQ(capture.text(), "owned:source-changed\n");
    }

    ScopedLogCapture capture;
    logger->log(namica::LogLevel::Info, "source-destroyed");
    EXPECT_EQ(capture.text(), "owned:source-destroyed\n");
}

// 独立创建的实例不应互相覆盖配置；其中一个销毁后，另一个仍能继续输出。
TEST_F(NamicaRuntimeTest, LoggerKeepsNewInstancesIndependent)
{
    auto second{namica::Logger::create()};
    ASSERT_TRUE(second->setLogFmt("second:{2}"));
    second->setLogMinLevel(namica::LogLevel::Debug);
    ScopedLogCapture capture;

    {
        auto first{namica::Logger::create()};
        ASSERT_TRUE(first->setLogFmt("first:{2}"));
        first->setLogMinLevel(namica::LogLevel::Error);
        first->log(namica::LogLevel::Info, "hidden");
        second->log(namica::LogLevel::Debug, "debug");
        first->log(namica::LogLevel::Error, "error");
    }

    second->log(namica::LogLevel::Info, "still-alive");

    EXPECT_EQ(capture.text(), "second:debug\nfirst:error\nsecond:still-alive\n");
}

// 使用错误的动态格式检测过滤时机：已拒绝的消息不应进入格式解析。
TEST_F(NamicaRuntimeTest, LoggerSkipsMalformedFilteredMessages)
{
    auto logger{namica::Logger::create()};
    logger->setLogMinLevel(namica::LogLevel::Info);
    namica::String malformedFormat{"bad {"};
    ScopedLogCapture capture;

    // 检查动态格式的运行期过滤；不约束未来字面量格式的编译期检查。
    EXPECT_NO_THROW(logger->log(namica::LogLevel::Debug, malformedFormat));
    EXPECT_TRUE(capture.text().empty());
}

// 计数的是 formatter 的执行次数：被过滤时为零，正常接收时只执行一次。
TEST_F(NamicaRuntimeTest, LoggerSkipsFormattingFilteredArguments)
{
    auto logger{namica::Logger::create()};
    ASSERT_TRUE(logger->setLogFmt("{2}"));
    logger->setLogMinLevel(namica::LogLevel::Info);
    namica::Int formatCalls{0};
    LoggerFormatProbe value{formatCalls};
    ScopedLogCapture capture;

    logger->log(namica::LogLevel::Debug, "hidden={}", value);
    EXPECT_EQ(formatCalls, 0);
    EXPECT_TRUE(capture.text().empty());

    logger->log(namica::LogLevel::Info, "visible={}", value);
    EXPECT_EQ(formatCalls, 1);
    EXPECT_EQ(capture.text(), "visible=7\n");
}
