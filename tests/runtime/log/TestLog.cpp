#include "runtime/utils/TestUtils.h"
#include "runtime/log/utils/FileTestUtils.h"
#include "runtime/log/utils/StreamTestUtils.h"

#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/core/Types.h>
#include <namica/log/Log.h>
#include <namica/log/Logger.h>
#include <namica/log/LogRecord.h>
#include <namica/log/sink/LogSink.h>
#include <namica/log/sink/LogSinkConfig.h>

#include <array>
#include <filesystem>
#include <format>
#include <vector>

namespace
{

// 用引用观察实际 formatter 的执行次数，固定输出 7 便于核对派发内容。
// 这只能验证 Logger 是否跳过/复用格式化，不能证明调用前的实参没有被求值。
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

// 经公开 LogSink 接口接收记录，不依赖 Logger 的私有状态或真实输出设备。
// write 保存副本以检查内容和顺序，flush 仅计数且不清空记录；二者均模拟成功。
// records() 返回借用引用，读取时需保留 sink；对象构造/析构同时覆盖 DLL 扩展入口。
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

// 将六个等级分别设为接收阈值，每组都按 Trace 到 Critical 提交全部等级。
// 预期只接收等于或高于阈值的记录，并保留提交顺序；六组覆盖两端极值和相等边界。
// 预期列表显式给出，避免复制生产代码的比较逻辑后与实现一起算错。
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

// 显式注册一个使用默认 Trace 阈值的 RecordingSink，再提交全部六个等级。
// 预期每级恰好收到一次且顺序不变；这里验证端的默认阈值，不表示 Logger 自动创建端。
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

// 注册后先把阈值提高到 Error，再降低到 Debug，分别提交边界等级及更低等级。
// 预期只保留 Error、Debug 两条，并能查询到最新阈值，防止 Logger 缓存过期过滤条件。
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

// 同时传入具名字符串、整数和浮点变量，验证正文参数格式化支持常见左值类型。
// 参数索引、整数补零和小数精度都应生效，整条结果只派发一次且等于指定文本。
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

// 直接把临时整数传给 log，覆盖模板转发与标准格式化参数构造的合法调用。
// 调用必须可编译，并产生一条 value=42，防止接口调整后只支持具名变量。
TEST_F(NamicaRuntimeTest, LoggerFormatsIntegerRvalues)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "value={}", namica::Int{42});

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "value=42");
}

// 将临时 String 作为参数，调用结束后它已销毁，再检查 sink 保存的正文。
// 预期仍得到完整的 value=temporary，验证格式化在实参有效期内完成且记录拥有结果。
TEST_F(NamicaRuntimeTest, LoggerFormatsTemporaryStrings)
{
    auto logger{namica::Logger::create()};
    auto sink{std::make_shared<RecordingSink>()};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "value={}", namica::String{"temporary"});

    ASSERT_EQ(sink->records().size(), std::size_t{1});
    EXPECT_EQ(sink->records().front().message, "value=temporary");
}

// 使用可修改的 String 作格式串，两次调用之间更换布局与补零规则。
// 预期两条消息分别使用各自调用时的格式，支持动态格式而不复用上一次解析结果。
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

// 中文内容经格式参数传入，格式串另外包含转义花括号 {{ready}}。
// 预期中文保持原样且双括号变成一对字面括号，验证正文格式化的 UTF-8 与转义语义。
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

// 向可接收 Warning 的端提交空格式串；空正文仍是一条有效日志记录。
// 预期记录数量为一、正文为空且等级保留，防止把空内容误当作没有日志而丢弃。
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

// Logger 未注册任何端时，提交带计数 formatter 的参数并显式 flush。
// 预期 formatter 不执行，空集合刷新正常返回；验证无输出目标时跳过无用格式化。
TEST_F(NamicaRuntimeTest, LoggerSkipsFormattingWithoutSinks)
{
    auto logger{namica::Logger::create()};
    namica::Int formatCalls{0};
    LoggerFormatProbe value{formatCalls};

    logger->log(namica::LogLevel::Info, "hidden={}", value);
    EXPECT_NO_THROW(logger->flush());

    EXPECT_EQ(formatCalls, 0);
}

// 注册 Warning、Error 两个阈值的端，再提交更低的 Debug 记录。
// 预期两端均无记录且 formatter 次数为零，区分提前过滤与格式化后才丢弃。
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

// 首个注册端拒收 Info，后续两个端接收，以计数 formatter 观察同一条消息的处理。
// 预期首端为空、后两端各有一条相同正文/来源/等级/时间文本，formatter 总共只执行一次。
// 这样同时防止遇到拒收端就停止遍历，以及为每个接收端重复格式化正文。
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

// 创建 Logger 后改写原来源字符串，提交消息后再改写原正文，并销毁 Logger。
// sink 独立保留记录，预期仍能读到创建时的来源和提交时的正文，而不是后来修改的值。
// 检查的是记录数据所有权，不要求 log() 参数本身在调用后继续存活。
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

// 先尝试注册空指针，再将同一个 sink 注册两次；只有第一次有效注册应成功。
// 随后提交并刷新，预期只有一条记录和一次刷新，避免重复注册导致重复输出。
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

// 注册两个端，检查移除空值、未知对象和已移除对象均失败，再观察正常移除后的派发。
// 被移除端不再收到 write/flush，保留端仍正常；重新注册原对象后应恢复接收后续记录。
// 预期重新注册不会补发移除期间的日志，也不影响另一个端已有的记录。
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

// 注册端后销毁调用方的 SharedPtr，只用 WeakPtr 观察其存活和记录内容。
// Logger 持有期间仍能写入；解除最后一处注册后 WeakPtr 应失效，验证持有与释放边界。
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

// 两个来源不同的 Logger 注册同一端，再从第一个移除并销毁第一个 Logger。
// 预期移除后它的消息不再出现，第二个仍可写入；共享对象不意味着共享注册列表。
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

// 两个端中只有一个接收 Info 消息，随后统一调用 Logger::flush。
// 预期两端都收到刷新，包括没有接收本条消息的高阈值端；已保存记录保持原样。
// 刷新针对输出端集合，不受当前消息过滤影响，也不是清空历史记录的接口。
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

// 通过 Application 正式入口配置 Core/App 来源，并各自注册可观察的接收端。
// 修改原 Config 后提交消息，预期仍使用启动时来源；退出作用域后两端收到刷新，
// 仅由 Log 持有的临时端被释放。用 Info 阈值排除内部启动/关闭诊断，不访问私有 init/clear。
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

// 启用 Truncate 文件端并关闭逐条刷新，Core/App 交替提交四条日志后正常关闭 Application。
// 预期旧文件内容被清除，四条记录按提交顺序完整保存且文件可删除，验证共享输出与关闭。
// 不固定内部刷新次数，也不依赖启动/关闭诊断的具体文本或编译期开关。
TEST_F(NamicaRuntimeTest, LogSharesConfiguredFileBetweenCoreAndApp)
{
    namica::test::log::TemporaryDirectory directory{};
    auto const path{directory.path() / "application.log"};
    namica::test::log::writeBinaryFile(path, "old contents\n");
    {
        namica::ApplicationConfig config{};
        config.logConfig.haveConsoleSink = namica::False;
        config.logConfig.haveFileSink = namica::True;
        config.logConfig.coreLoggerSource = "Runtime";
        config.logConfig.appLoggerSource = "Game";
        config.logConfig.fileSinkConfig.filePath = path;
        config.logConfig.fileSinkConfig.logFmt = "{2}|{3}";
        // 排除启动/关闭诊断，不依赖 DLL 的 Trace/Debug 编译开关或具体诊断措辞。
        config.logConfig.fileSinkConfig.minLevel = namica::LogLevel::Info;
        config.logConfig.fileSinkConfig.openMode = namica::FileOpenMode::Truncate;
        config.logConfig.fileSinkConfig.flushOnWrite = namica::False;
        namica::Application app{config};

        NAMICA_CORE_INFO("first");
        NAMICA_APP_INFO("second");
        NAMICA_CORE_WARNING("third");
        NAMICA_APP_ERROR("fourth");
    }

    // 两个独立缓冲 writer 指向同一路径可能覆盖或重排；共享端应保留提交顺序。
    EXPECT_EQ(namica::test::log::readBinaryFile(path),
              "Runtime|first\nGame|second\nRuntime|third\nGame|fourth\n");
    EXPECT_TRUE(std::filesystem::remove(path));
}

// 同时启用控制台与文件，分别设置 Warning/Info 阈值和不同的文本前缀。
// 预期 Info 只进文件，Warning/Error 进入两端且分别使用各自布局，来源字段保持一致。
// 控制台使用 Never 和内存捕获，文件使用独占临时目录，断言不依赖真实终端表现。
TEST_F(NamicaRuntimeTest, LogConfiguresConsoleAndFileIndependently)
{
    namica::test::log::TemporaryDirectory directory{};
    auto const path{directory.path() / "destinations.log"};
    namica::test::log::ScopedLogCapture console{};
    {
        namica::ApplicationConfig config{};
        config.logConfig.haveConsoleSink = namica::True;
        config.logConfig.consoleSinkConfig.minLevel = namica::LogLevel::Warning;
        config.logConfig.consoleSinkConfig.logFmt = "console:{2}|{3}";
        config.logConfig.consoleSinkConfig.colorMode = namica::ConsoleColorMode::Never;
        config.logConfig.haveFileSink = namica::True;
        config.logConfig.fileSinkConfig.filePath = path;
        config.logConfig.fileSinkConfig.minLevel = namica::LogLevel::Info;
        config.logConfig.fileSinkConfig.logFmt = "file:{2}|{3}";
        config.logConfig.fileSinkConfig.flushOnWrite = namica::False;
        namica::Application app{config};

        NAMICA_CORE_INFO("file only");
        NAMICA_APP_WARNING("both");
        NAMICA_CORE_ERROR("core error");
    }

    EXPECT_EQ(console.text(), "console:App|both\nconsole:Core|core error\n");
    EXPECT_EQ(namica::test::log::readBinaryFile(path),
              "file:Core|file only\nfile:App|both\nfile:Core|core error\n");
}

// 文件端关闭时故意提供无效布局和不存在的父目录，再创建应用并提交日志。
// 预期初始化与关闭均正常，父目录始终不存在；未启用的端不应校验配置或产生文件副作用。
TEST_F(NamicaRuntimeTest, LogDoesNotCreateOrValidateDisabledFileSink)
{
    namica::test::log::TemporaryDirectory directory{};
    auto const path{directory.path() / "uncreated" / "disabled.log"};
    {
        namica::ApplicationConfig config{};
        config.logConfig.haveConsoleSink = namica::False;
        config.logConfig.haveFileSink = namica::False;
        config.logConfig.fileSinkConfig.filePath = path;
        config.logConfig.fileSinkConfig.logFmt = "{";
        namica::Application app{config};

        NAMICA_APP_INFO("no file output");
    }

    EXPECT_FALSE(std::filesystem::exists(path.parent_path()));
}
