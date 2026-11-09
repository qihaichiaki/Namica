#include "runtime/utils/TestUtils.h"

#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/core/Types.h>
#include <namica/log/Log.h>
#include <namica/log/Logger.h>
#include <namica/log/LogRecord.h>
#include <namica/log/sink/LogSink.h>
#include <namica/log/sink/LogSinkConfig.h>

#include <array>
#include <format>
#include <vector>

namespace
{

// 只计数 formatter 的执行，不把调用前的参数求值当作格式化。
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
        return this->std::formatter<namica::Int>::format(namica::Int{7}, _context);
    }
};

namespace
{

// 经公开扩展接口接收并复制记录，同时验证客户端可调用基类构造和析构。
class RecordingSink final : public namica::LogSink
{
public:
    explicit RecordingSink(namica::LogLevel _minimum = namica::LogLevel::Trace)
        : LogSink{namica::LogSinkConfig{_minimum}}
    {
    }

    namica::Bool write(namica::LogRecord const& _record) override
    {
        this->m_records.push_back(_record);
        return namica::True;
    }

    namica::Bool flush() override
    {
        ++this->m_flushCalls;
        return namica::True;
    }

    [[nodiscard]] std::vector<namica::LogRecord> const& records() const noexcept
    {
        return this->m_records;
    }

    [[nodiscard]] namica::Int flushCalls() const noexcept
    {
        return this->m_flushCalls;
    }

private:
    std::vector<namica::LogRecord> m_records{};
    namica::Int m_flushCalls{0};
};

constexpr std::array kLogLevels{namica::LogLevel::Trace,
                                namica::LogLevel::Debug,
                                namica::LogLevel::Info,
                                namica::LogLevel::Warning,
                                namica::LogLevel::Error,
                                namica::LogLevel::Critical};

}  // namespace

// 显式列出阈值结果，覆盖等于阈值的边界，不在测试里重写过滤算法。
TEST_F(NamicaRuntimeTest, LoggerFiltersAtEveryThreshold)
{
    struct FilterCase
    {
        namica::LogLevel minimum;
        std::vector<namica::LogLevel> expected;
    };

    using namica::LogLevel;
    std::array const cases{
        FilterCase{LogLevel::Trace,
                   {LogLevel::Trace,
                    LogLevel::Debug,
                    LogLevel::Info,
                    LogLevel::Warning,
                    LogLevel::Error,
                    LogLevel::Critical}},
        FilterCase{LogLevel::Debug,
                   {LogLevel::Debug,
                    LogLevel::Info,
                    LogLevel::Warning,
                    LogLevel::Error,
                    LogLevel::Critical}},
        FilterCase{LogLevel::Info,
                   {LogLevel::Info, LogLevel::Warning, LogLevel::Error, LogLevel::Critical}},
        FilterCase{LogLevel::Warning, {LogLevel::Warning, LogLevel::Error, LogLevel::Critical}},
        FilterCase{LogLevel::Error, {LogLevel::Error, LogLevel::Critical}},
        FilterCase{LogLevel::Critical, {LogLevel::Critical}}};

    for (auto const& filterCase : cases)
    {
        SCOPED_TRACE(static_cast<namica::Int>(filterCase.minimum));
        auto logger{namica::Logger::create()};
        auto sink{std::make_shared<RecordingSink>(filterCase.minimum)};
        ASSERT_TRUE(logger->addSink(sink));
        EXPECT_EQ(sink->getLogMinLevel(), filterCase.minimum);

        for (auto level : kLogLevels)
        {
            logger->log(level, "message");
        }

        std::vector<LogLevel> received{};
        for (auto const& record : sink->records())
        {
            received.push_back(record.logLevel);
        }
        EXPECT_EQ(received, filterCase.expected);
    }
}

TEST_F(NamicaRuntimeTest, LoggerAcceptsAllLevelsThroughDefaultSink)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    for (auto level : kLogLevels)
    {
        logger->log(level, "message");
    }

    ASSERT_EQ(sink->records().size(), kLogLevels.size());
    for (std::size_t index{0}; index < kLogLevels.size(); ++index)
    {
        EXPECT_EQ(sink->records()[index].logLevel, kLogLevels[index]);
    }
}

TEST_F(NamicaRuntimeTest, LoggerUsesUpdatedSinkMinimumLevel)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    sink->setLogMinLevel(namica::LogLevel::Error);
    logger->log(namica::LogLevel::Warning, "hidden-warning");
    logger->log(namica::LogLevel::Error, "visible-error");
    sink->setLogMinLevel(namica::LogLevel::Debug);
    logger->log(namica::LogLevel::Debug, "visible-debug");
    logger->log(namica::LogLevel::Trace, "hidden-trace");

    ASSERT_EQ(sink->records().size(), std::size_t{2});
    EXPECT_EQ(sink->records()[0].message, "visible-error");
    EXPECT_EQ(sink->records()[1].message, "visible-debug");
    EXPECT_EQ(sink->getLogMinLevel(), namica::LogLevel::Debug);
}

TEST_F(NamicaRuntimeTest, LoggerFormatsLvalueArguments)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));
    namica::String const name{"snake"};
    namica::Int count{42};
    namica::Double duration{3.5};

    logger->log(namica::LogLevel::Info, "{0}:{1:04d}:{2:.1f}", name, count, duration);

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "snake:0042:3.5");
}

TEST_F(NamicaRuntimeTest, LoggerFormatsIntegerRvalues)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "value={}", namica::Int{42});

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "value=42");
}

TEST_F(NamicaRuntimeTest, LoggerFormatsTemporaryStrings)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "value={}", namica::String{"temporary"});

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "value=temporary");
}

TEST_F(NamicaRuntimeTest, LoggerAcceptsRuntimeMessageFormats)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));
    namica::String messageFormat{"value={}"};
    namica::Int value{12};

    logger->log(namica::LogLevel::Info, messageFormat, value);
    messageFormat = "next={:03d}";
    logger->log(namica::LogLevel::Info, messageFormat, value);

    ASSERT_EQ(sink->records().size(), std::size_t{2});
    EXPECT_EQ(sink->records()[0].message, "value=12");
    EXPECT_EQ(sink->records()[1].message, "next=012");
}

TEST_F(NamicaRuntimeTest, LoggerPreservesUtf8AndEscapedBraces)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));
    namica::String asset{"贪吃蛇"};

    logger->log(namica::LogLevel::Info, "资源：{} {{ready}}", asset);

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "资源：贪吃蛇 {ready}");
}

TEST_F(NamicaRuntimeTest, LoggerAcceptsEmptyMessages)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Warning, "");

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_TRUE(sink->records().front().message.empty());
    EXPECT_EQ(sink->records().front().logLevel, namica::LogLevel::Warning);
}

// 无接收端与全部拒收都不能执行 formatter；不约束尚未定下的格式错误报告。
TEST_F(NamicaRuntimeTest, LoggerSkipsFormattingWithoutSinks)
{
    auto logger{namica::Logger::create()};
    namica::Int formatCalls{0};
    LoggerFormatProbe value{formatCalls};

    logger->log(namica::LogLevel::Info, "hidden={}", value);
    EXPECT_NO_THROW(logger->flush());

    EXPECT_EQ(formatCalls, 0);
}

TEST_F(NamicaRuntimeTest, LoggerSkipsFormattingWhenAllSinksReject)
{
    auto logger{namica::Logger::create()};
    auto first{std::make_shared<RecordingSink>(namica::LogLevel::Warning)};
    auto second{std::make_shared<RecordingSink>(namica::LogLevel::Error)};
    ASSERT_TRUE(logger->addSink(first));
    ASSERT_TRUE(logger->addSink(second));
    namica::Int formatCalls{0};
    LoggerFormatProbe value{formatCalls};

    logger->log(namica::LogLevel::Debug, "hidden={}", value);

    EXPECT_EQ(formatCalls, 0);
    EXPECT_TRUE(first->records().empty());
    EXPECT_TRUE(second->records().empty());
}

// 首个端拒收不应妨碍后续端接收；多个端接收时正文仍只格式化一次。
TEST_F(NamicaRuntimeTest, LoggerFormatsOnceAndFiltersEachSinkIndependently)
{
    auto logger{namica::Logger::create("Gameplay")};
    auto rejected{std::make_shared<RecordingSink>(namica::LogLevel::Error)};
    auto first{std::make_shared<RecordingSink>(namica::LogLevel::Info)};
    auto second{std::make_shared<RecordingSink>(namica::LogLevel::Debug)};
    ASSERT_TRUE(logger->addSink(rejected));
    ASSERT_TRUE(logger->addSink(first));
    ASSERT_TRUE(logger->addSink(second));
    namica::Int formatCalls{0};
    LoggerFormatProbe value{formatCalls};

    logger->log(namica::LogLevel::Info, "visible={}", value);

    EXPECT_EQ(formatCalls, 1);
    EXPECT_TRUE(rejected->records().empty());
    ASSERT_EQ(first->records().size(), std::size_t{1});
    ASSERT_EQ(second->records().size(), std::size_t{1});
    for (auto const& sink : {first, second})
    {
        auto const& record{sink->records().front()};
        EXPECT_EQ(record.message, "visible=7");
        EXPECT_EQ(record.source, "Gameplay");
        EXPECT_EQ(record.logLevel, namica::LogLevel::Info);
    }
    EXPECT_EQ(first->records().front().timePoint.getStr(NAMICA_TIME_YMD_HMS),
              second->records().front().timePoint.getStr(NAMICA_TIME_YMD_HMS));
}

// 接收端保存的是副本；调用参数、来源字符串和 Logger 销毁后仍可读取。
TEST_F(NamicaRuntimeTest, LoggerOwnsSourceAndProducesOwnedRecordContents)
{
    auto sink{std::make_shared<RecordingSink>()};
    {
        namica::String source{"Gameplay"};
        auto logger{namica::Logger::create(source)};
        source = "Changed";
        ASSERT_TRUE(logger->addSink(sink));
        namica::String message{"original"};
        logger->log(namica::LogLevel::Info, "{}", message);
        message = "modified";
        logger->log(namica::LogLevel::Warning, "next");
    }

    ASSERT_EQ(sink->records().size(), std::size_t{2});
    EXPECT_EQ(sink->records()[0].source, "Gameplay");
    EXPECT_EQ(sink->records()[0].message, "original");
    EXPECT_EQ(sink->records()[1].message, "next");
}

TEST_F(NamicaRuntimeTest, LoggerRejectsNullAndDuplicateSinks)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    EXPECT_FALSE(logger->addSink(nullptr));
    ASSERT_TRUE(logger->addSink(sink));
    EXPECT_FALSE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "once");
    logger->flush();

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->flushCalls(), 1);
}

TEST_F(NamicaRuntimeTest, LoggerRemovesAndCanRegisterSinkAgain)
{
    auto logger{namica::Logger::create()};
    auto removed{std::make_shared<RecordingSink>()};
    auto retained{std::make_shared<RecordingSink>()};
    auto unknown{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(removed));
    ASSERT_TRUE(logger->addSink(retained));
    EXPECT_FALSE(logger->removeSink(nullptr));
    EXPECT_FALSE(logger->removeSink(unknown));
    ASSERT_TRUE(logger->removeSink(removed));
    EXPECT_FALSE(logger->removeSink(removed));

    logger->log(namica::LogLevel::Info, "after-remove");
    logger->flush();
    EXPECT_TRUE(removed->records().empty());
    EXPECT_EQ(removed->flushCalls(), 0);
    ASSERT_EQ(retained->records().size(), std::size_t{1});
    EXPECT_EQ(retained->flushCalls(), 1);

    ASSERT_TRUE(logger->addSink(removed));
    logger->log(namica::LogLevel::Info, "after-register");
    ASSERT_EQ(removed->records().size(), std::size_t{1});
    EXPECT_EQ(removed->records().front().message, "after-register");
    EXPECT_EQ(retained->records().size(), std::size_t{2});
}

TEST_F(NamicaRuntimeTest, LoggerKeepsRegisteredSinkAliveUntilRemoval)
{
    auto logger{namica::Logger::create()};
    namica::WeakPtr<RecordingSink> observed{};
    {
        auto sink{std::make_shared<RecordingSink>()};
        observed = sink;
        ASSERT_TRUE(logger->addSink(sink));
    }

    ASSERT_FALSE(observed.expired());
    logger->log(namica::LogLevel::Info, "still-alive");
    EXPECT_EQ(observed.lock()->records().size(), std::size_t{1});
    ASSERT_TRUE(logger->removeSink(observed.lock()));
    EXPECT_TRUE(observed.expired());
}

TEST_F(NamicaRuntimeTest, LoggerCanShareSinkWithoutSharingRegistration)
{
    auto first{namica::Logger::create("First")};
    auto second{namica::Logger::create("Second")};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(first->addSink(sink));
    ASSERT_TRUE(second->addSink(sink));
    first->log(namica::LogLevel::Info, "first");
    ASSERT_TRUE(first->removeSink(sink));
    first->log(namica::LogLevel::Info, "hidden");
    first.reset();
    second->log(namica::LogLevel::Info, "second");

    ASSERT_EQ(sink->records().size(), std::size_t{2});
    EXPECT_EQ(sink->records()[0].source, "First");
    EXPECT_EQ(sink->records()[1].source, "Second");
}

// flush 与等级过滤无关，也不应清空接收端已经保存的记录。
TEST_F(NamicaRuntimeTest, LoggerFlushesEveryRegisteredSinkWithoutClearingRecords)
{
    auto logger{namica::Logger::create()};
    auto first{std::make_shared<RecordingSink>()};
    auto second{std::make_shared<RecordingSink>(namica::LogLevel::Critical)};
    ASSERT_TRUE(logger->addSink(first));
    ASSERT_TRUE(logger->addSink(second));
    logger->log(namica::LogLevel::Info, "saved");

    logger->flush();

    EXPECT_EQ(first->flushCalls(), 1);
    EXPECT_EQ(second->flushCalls(), 1);
    ASSERT_EQ(first->records().size(), std::size_t{1});
    EXPECT_EQ(first->records().front().message, "saved");
    EXPECT_TRUE(second->records().empty());
}

// 通过 Application 的正式入口验证 Core/App 来源及正常关闭，不访问私有 init/clear。
TEST_F(NamicaRuntimeTest, LogUsesApplicationConfigAndFlushesOnShutdown)
{
    auto coreSink{std::make_shared<RecordingSink>(namica::LogLevel::Info)};
    auto appSink{std::make_shared<RecordingSink>(namica::LogLevel::Info)};
    namica::WeakPtr<RecordingSink> lifetime{};
    {
        namica::ApplicationConfig config{};
        config.logConfig.haveConsoleSink = namica::False;
        config.logConfig.coreLoggerSource = "Runtime";
        config.logConfig.appLoggerSource = "Game";
        namica::Application app{config};
        ASSERT_TRUE(namica::Log::getCoreLogger().addSink(coreSink));
        ASSERT_TRUE(namica::Log::getAppLogger().addSink(appSink));
        {
            auto sink{std::make_shared<RecordingSink>()};
            lifetime = sink;
            ASSERT_TRUE(namica::Log::getAppLogger().addSink(sink));
        }
        config.logConfig.coreLoggerSource = "Changed";
        config.logConfig.appLoggerSource = "Changed";

        NAMICA_CORE_INFO("core={}", namica::Int{1});
        NAMICA_APP_WARNING("app={}", namica::Int{2});
        EXPECT_FALSE(lifetime.expired());
    }

    ASSERT_EQ(coreSink->records().size(), std::size_t{1});
    ASSERT_EQ(appSink->records().size(), std::size_t{1});
    EXPECT_EQ(coreSink->records().front().source, "Runtime");
    EXPECT_EQ(coreSink->records().front().message, "core=1");
    EXPECT_EQ(coreSink->records().front().logLevel, namica::LogLevel::Info);
    EXPECT_EQ(appSink->records().front().source, "Game");
    EXPECT_EQ(appSink->records().front().message, "app=2");
    EXPECT_EQ(appSink->records().front().logLevel, namica::LogLevel::Warning);
    EXPECT_GE(coreSink->flushCalls(), 1);
    EXPECT_GE(appSink->flushCalls(), 1);
    EXPECT_TRUE(lifetime.expired());
}
