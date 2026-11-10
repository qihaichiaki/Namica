#include "runtime/utils/TestUtils.h"
#include "runtime/log/utils/FileTestUtils.h"

#include <namica/core/Types.h>
#include <namica/log/Logger.h>
#include <namica/log/LogRecord.h>
#include <namica/log/sink/FileSink.h>
#include <namica/log/sink/FileSinkConfig.h>

#include <filesystem>
#include <format>
#include <stdexcept>
#include <type_traits>

namespace
{

using namica::test::log::readBinaryFile;
using namica::test::log::TemporaryDirectory;
using namica::test::log::writeBinaryFile;

// 编译期守住 FileSink 对象的身份约束，防止已注册对象的文件资源被复制或移走。
// 此处约束对象本身；复制、移动 SharedPtr 仍是共享所有权的正常用法。
static_assert(!std::is_copy_constructible_v<namica::FileSink>);
static_assert(!std::is_copy_assignable_v<namica::FileSink>);
static_assert(!std::is_move_constructible_v<namica::FileSink>);
static_assert(!std::is_move_assignable_v<namica::FileSink>);

// 将输出简化为正文，便于精确比较文件字节；打开模式和刷新策略保留默认值。
// 本文件专用的配置准备保留局部，跨日志测试文件复用的文件操作才放到 log/utils。
namica::FileSinkConfig makeFileConfig(std::filesystem::path const& _path)
{
    namica::FileSinkConfig config{};
    config.filePath = _path;
    config.logFmt = "{3}";
    return config;
}

// 只覆盖正文，保留 LogRecord 自身的默认等级、来源与记录时间。
namica::LogRecord makeRecord(namica::String const& _message)
{
    namica::LogRecord record{};
    record.message = _message;
    return record;
}

}  // namespace

// 在不存在的多级父目录中创建日志，路径同时包含中文、空格和中文文件名。
// 预期 FileSink 自动补齐目录，并在默认逐条刷新下立即读到完整中文正文和一个 LF；
// 使用独立临时目录，避免已有目录掩盖创建逻辑，也不依赖开发者的本地路径。
TEST_F(NamicaRuntimeTest, FileSinkCreatesParentDirectoriesAndSupportsUnicodePaths)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / std::filesystem::path{u8"日志 目录"} / "nested" /
                    std::filesystem::path{u8"运行.log"}};
    ASSERT_FALSE(std::filesystem::exists(path.parent_path()));
    auto sink{namica::FileSink::create(makeFileConfig(path))};

    ASSERT_TRUE(sink->write(makeRecord("创建成功")));

    EXPECT_TRUE(std::filesystem::is_directory(path.parent_path()));
    EXPECT_EQ(readBinaryFile(path), "创建成功\n");
}

// 验证默认 Append 保留已有字节，连续销毁、重新打开文件端后仍从末尾续写。
// 旧内容故意不以换行结尾，预期直接拼接两条新记录，不插入额外分隔符或重写旧内容。
TEST_F(NamicaRuntimeTest, FileSinkAppendsWithoutChangingExistingBytes)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "append.log"};
    writeBinaryFile(path, "existing|");
    auto const config{makeFileConfig(path)};
    // 两个作用域分别关闭文件再重新打开，覆盖跨 FileSink 实例的追加行为。
    {
        auto sink{namica::FileSink::create(config)};
        ASSERT_TRUE(sink->write(makeRecord("first")));
    }
    {
        auto sink{namica::FileSink::create(config)};
        ASSERT_TRUE(sink->write(makeRecord("second")));
    }

    EXPECT_EQ(readBinaryFile(path), "existing|first\nsecond\n");
}

// 对已有日志选择 Truncate，构造完成时旧内容就应被清空，无须等待第一条记录。
// 同一实例随后写入的两条记录应同时保留，防止误把“打开时截断”实现成“每次写入截断”。
TEST_F(NamicaRuntimeTest, FileSinkTruncatesOnCreationButKeepsSubsequentWrites)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "truncate.log"};
    writeBinaryFile(path, "previous contents\n");
    auto config{makeFileConfig(path)};
    config.openMode = namica::FileOpenMode::Truncate;
    auto sink{namica::FileSink::create(config)};
    EXPECT_EQ(readBinaryFile(path), "");

    ASSERT_TRUE(sink->write(makeRecord("first")));
    ASSERT_TRUE(sink->write(makeRecord("second")));

    EXPECT_EQ(readBinaryFile(path), "first\nsecond\n");
}

// 默认 FileSinkConfig 没有输出路径，不能据此创建可用文件端。
// 预期构造立即抛 std::invalid_argument，明确区分缺少必填参数与后续文件系统错误。
TEST_F(NamicaRuntimeTest, FileSinkRejectsEmptyPath)
{
    namica::FileSinkConfig config{};

    EXPECT_THROW(namica::FileSink::create(config), std::invalid_argument);
}

// 让普通文件占据输出路径中的父目录位置，稳定构造无法创建目录的场景。
// 预期抛 filesystem_error，并完整保留原文件；不依赖管理员权限、磁盘容量或只读属性。
TEST_F(NamicaRuntimeTest, FileSinkRejectsFileAsParentDirectory)
{
    TemporaryDirectory directory{};
    auto const parent{directory.path() / "ordinary-file"};
    writeBinaryFile(parent, "keep me");
    auto const config{makeFileConfig(parent / "nested" / "output.log")};

    EXPECT_THROW(namica::FileSink::create(config), std::filesystem::filesystem_error);
    EXPECT_EQ(readBinaryFile(parent), "keep me");
}

// 将已存在的目录本身作为日志文件路径，覆盖父目录合法但最终文件无法打开的情况。
// 预期抛 filesystem_error，原目录仍存在，不把目录当作可写文件或替换掉它。
TEST_F(NamicaRuntimeTest, FileSinkRejectsDirectoryAsOutputFile)
{
    TemporaryDirectory directory{};
    auto const config{makeFileConfig(directory.path())};

    EXPECT_THROW(namica::FileSink::create(config), std::filesystem::filesystem_error);
    EXPECT_TRUE(std::filesystem::is_directory(directory.path()));
}

// 无效打开模式必须在任何文件副作用之前被拒绝，不能兜底解释为 Truncate 等合法模式。
// 分别使用已有文件和不存在的父目录，预期均抛 invalid_argument，旧字节不变且不创建目录。
TEST_F(NamicaRuntimeTest, FileSinkRejectsInvalidModeBeforeChangingFiles)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "existing.log"};
    writeBinaryFile(path, "keep me\n");
    auto config{makeFileConfig(path)};
    // 显式构造枚举范围外的值，模拟调用端传入未经校验的启动配置。
    config.openMode = static_cast<namica::FileOpenMode>(-1);

    EXPECT_THROW(namica::FileSink::create(config), std::invalid_argument);
    EXPECT_EQ(readBinaryFile(path), "keep me\n");

    config.filePath = directory.path() / "uncreated" / "output.log";
    EXPECT_THROW(namica::FileSink::create(config), std::invalid_argument);
    EXPECT_FALSE(std::filesystem::exists(config.filePath.parent_path()));
}

// 布局格式与日期格式都属于启动校验，任一无效都应抛 format_error。
// 对两类错误分别验证已有文件的 Truncate 场景和全新目录场景，确保格式校验先于文件副作用；
// 旧日志必须保留，不存在的父目录也不能因为失败的初始化而被创建。
TEST_F(NamicaRuntimeTest, FileSinkValidatesFormatsBeforeCreatingOrTruncatingFiles)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "existing.log"};
    writeBinaryFile(path, "keep me\n");

    // 两轮只改变一类格式：未闭合的布局占位符，以及不完整的日期转换说明符。
    for (auto invalidLayout : {namica::True, namica::False})
    {
        SCOPED_TRACE(invalidLayout ? "layout" : "date format");
        auto config{makeFileConfig(path)};
        config.openMode = namica::FileOpenMode::Truncate;
        if (invalidLayout)
        {
            config.logFmt = "{";
        }
        else
        {
            config.dateFmt = "{:%}";
        }

        EXPECT_THROW(namica::FileSink::create(config), std::format_error);
        EXPECT_EQ(readBinaryFile(path), "keep me\n");
        config.filePath = directory.path() / "uncreated" / "output.log";
        EXPECT_THROW(namica::FileSink::create(config), std::format_error);
        EXPECT_FALSE(std::filesystem::exists(config.filePath.parent_path()));
    }
}

// 重排四个布局字段并指定日期格式，验证文件端输出的是 LogRecord 的时间、等级、来源和正文。
// 预期逐字节匹配指定顺序和一个行结束符，覆盖 TextSink 格式化结果到文件输出的完整路径。
TEST_F(NamicaRuntimeTest, FileSinkUsesRecordTimeLevelSourceAndMessage)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "layout.log"};
    auto config{makeFileConfig(path)};
    config.logFmt = "{2}|{1}|{0}|{3}";
    config.dateFmt = NAMICA_TIME_YMD_HMS;
    auto sink{namica::FileSink::create(config)};
    auto record{makeRecord("payload")};
    record.source = "Runtime";
    record.logLevel = namica::LogLevel::Warning;
    // 使用同一条记录的时间推导预期值，不重新读取时钟，避免跨秒造成偶发失败。
    auto const expected{"Runtime|Warning|" + record.timePoint.getStr(config.dateFmt) +
                        "|payload\n"};

    ASSERT_TRUE(sink->write(record));

    EXPECT_EQ(readBinaryFile(path), expected);
}

// 依次写入空正文、带中文/花括号/不同换行的正文和包含 NUL 的正文，检查二进制字节保真。
// 每次 write 仅在正文后追加一个 LF：原有 LF、CRLF 和 NUL 均保留，不添加 BOM 或颜色，
// 也不把正文花括号再次作为格式字符串解析；空正文仍应产生一条空行。
TEST_F(NamicaRuntimeTest, FileSinkPreservesUtf8NullBytesAndLineBreaks)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "bytes.log"};
    auto sink{namica::FileSink::create(makeFileConfig(path))};
    // 显式给出长度，否则 String 会在构造时就截断于 NUL，无法验证文件端是否保留它。
    namica::String const embeddedNull{"A\0B", std::size_t{3}};

    ASSERT_TRUE(sink->write(makeRecord("")));
    ASSERT_TRUE(sink->write(makeRecord("资源：贪吃蛇 {ready}\nnext\r\n")));
    ASSERT_TRUE(sink->write(makeRecord(embeddedNull)));

    // 正文已以 CRLF 结尾也仍追加本条记录的 LF，预期值刻意保留这两个相邻换行。
    auto const expected{namica::String{"\n资源：贪吃蛇 {ready}\nnext\r\n\n"} + embeddedNull + "\n"};
    EXPECT_EQ(readBinaryFile(path), expected);
}

// 创建文件端后修改并销毁原配置，验证路径、布局、日期格式和最低等级由文件端独立持有。
// 随后的写入应继续使用原文件和原格式，且默认逐条刷新仍生效；新路径不得被创建。
// 此处直接调用 write 检查文本配置，最低等级通过 getter 验证，派发筛选由 Logger 用例覆盖。
TEST_F(NamicaRuntimeTest, FileSinkOwnsStartupPathAndTextConfiguration)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "owned.log"};
    auto const unusedPath{directory.path() / "unused.log"};
    namica::SharedPtr<namica::FileSink> sink{};
    // 让配置先于 sink 离开作用域，防止实现错误保存配置引用却在普通调用中未暴露。
    {
        auto config{makeFileConfig(path)};
        config.logFmt = "owned:{0}|{3}";
        config.dateFmt = NAMICA_TIME_YMD;
        config.minLevel = namica::LogLevel::Warning;
        sink = namica::FileSink::create(config);
        config.filePath = unusedPath;
        config.logFmt = "changed:{3}";
        config.dateFmt = NAMICA_TIME_HMS;
        config.minLevel = namica::LogLevel::Critical;
        config.flushOnWrite = namica::False;
    }
    auto const record{makeRecord("payload")};

    ASSERT_TRUE(sink->write(record));

    EXPECT_EQ(readBinaryFile(path),
              "owned:" + record.timePoint.getStr(NAMICA_TIME_YMD) + "|payload\n");
    EXPECT_FALSE(std::filesystem::exists(unusedPath));
    EXPECT_EQ(sink->getLogMinLevel(), namica::LogLevel::Warning);
}

// 运行时成功设置格式后，修改并销毁传入的字符串，输出仍应使用设置时的内容。
// 随后分别提交无效布局和日期格式，预期返回 False 且保留最后一组合法格式，不能部分覆盖状态。
TEST_F(NamicaRuntimeTest, FileSinkUsesOwnedRuntimeFormatsAndRejectsInvalidUpdates)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "formats.log"};
    auto sink{namica::FileSink::create(makeFileConfig(path))};
    {
        namica::String layout{"updated:{0}|{2}|{3}"};
        namica::String dateFormat{NAMICA_TIME_YMD};
        ASSERT_TRUE(sink->setLogFmt(layout));
        ASSERT_TRUE(sink->setDateFmt(dateFormat));
        layout = "changed:{3}";
        dateFormat = NAMICA_TIME_HMS;
    }
    ASSERT_FALSE(sink->setLogFmt("{"));
    ASSERT_FALSE(sink->setDateFmt("{:%}"));
    auto record{makeRecord("payload")};
    record.source = "App";

    ASSERT_TRUE(sink->write(record));

    EXPECT_EQ(readBinaryFile(path),
              "updated:" + record.timePoint.getStr(NAMICA_TIME_YMD) + "|App|payload\n");
}

// 关闭逐条刷新后，显式 flush 仍须使之前接受的记录可从文件读到。
// 同时覆盖空文件刷新、连续重复刷新和刷新后的继续写入，预期始终成功且无重复内容；
// 这里只验证流缓冲提交后的可见性，不承诺断电持久化，也不要求刷新前文件必须为空。
TEST_F(NamicaRuntimeTest, FileSinkFlushesPendingWritesAndCanFlushRepeatedly)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "manual-flush.log"};
    auto config{makeFileConfig(path)};
    config.flushOnWrite = namica::False;
    auto sink{namica::FileSink::create(config)};
    ASSERT_TRUE(sink->flush());
    EXPECT_EQ(readBinaryFile(path), "");
    ASSERT_TRUE(sink->write(makeRecord("first")));
    ASSERT_TRUE(sink->write(makeRecord("second")));

    // 不约束刷新前的文件长度，标准库可以自行提交缓冲。
    ASSERT_TRUE(sink->flush());
    EXPECT_EQ(readBinaryFile(path), "first\nsecond\n");
    ASSERT_TRUE(sink->flush());
    EXPECT_EQ(readBinaryFile(path), "first\nsecond\n");
    ASSERT_TRUE(sink->write(makeRecord("third")));
    ASSERT_TRUE(sink->flush());
    EXPECT_EQ(readBinaryFile(path), "first\nsecond\nthird\n");
}

// 在同一文件端上先关闭、再开启、再关闭逐条刷新，验证运行时策略切换不丢记录。
// 开启后的下一次 write 应让此前记录一起可读；再次关闭后，显式 flush 仍有效。
// 不检查 setter 自身是否立刻刷新，也不固定关闭期间标准库何时自行提交缓冲。
TEST_F(NamicaRuntimeTest, FileSinkCanChangeFlushOnWriteAtRuntime)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "flush-policy.log"};
    auto config{makeFileConfig(path)};
    config.flushOnWrite = namica::False;
    auto sink{namica::FileSink::create(config)};
    ASSERT_TRUE(sink->write(makeRecord("buffered")));

    sink->setFlushOnWrite(namica::True);
    ASSERT_TRUE(sink->write(makeRecord("automatic")));
    EXPECT_EQ(readBinaryFile(path), "buffered\nautomatic\n");

    sink->setFlushOnWrite(namica::False);
    ASSERT_TRUE(sink->write(makeRecord("manual")));
    ASSERT_TRUE(sink->flush());
    EXPECT_EQ(readBinaryFile(path), "buffered\nautomatic\nmanual\n");
}

// 关闭逐条刷新并写入记录后，直接释放最后一个持有者，不额外调用 flush。
// 正常文件系统条件下，析构关闭文件应保留全部待提交内容，并释放文件资源；
// 本用例不模拟析构期间的 I/O 故障，也不约束尚未确定的析构错误上报策略。
TEST_F(NamicaRuntimeTest, FileSinkClosesAndWritesRemainingDataOnDestruction)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "close.log"};
    auto config{makeFileConfig(path)};
    config.flushOnWrite = namica::False;
    {
        auto sink{namica::FileSink::create(config)};
        ASSERT_TRUE(sink->write(makeRecord("last record")));
    }

    EXPECT_EQ(readBinaryFile(path), "last record\n");
    // 当前 Windows/ofstream 组合下，成功删除还能检查句柄已关闭；
    // 不把其他平台允许删除已打开文件的行为作为失败条件。
    EXPECT_TRUE(std::filesystem::remove(path));
}

// 经 Logger 派发到 FileSink，先以 Warning 为阈值，再运行时提高到 Error。
// 预期只保留每一阶段达到阈值的记录，并正确填入 Logger 来源、等级和格式化后的正文。
// 文件端关闭逐条刷新，最后由 Logger::flush 提交，验证筛选和刷新沿公开接入链路生效。
TEST_F(NamicaRuntimeTest, FileSinkUsesLoggerFilteringAndExplicitFlush)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "filtered.log"};
    auto config{makeFileConfig(path)};
    config.minLevel = namica::LogLevel::Warning;
    config.logFmt = "{2}:{1}|{3}";
    config.flushOnWrite = namica::False;
    auto sink{namica::FileSink::create(config)};
    auto logger{namica::Logger::create("Gameplay")};
    ASSERT_TRUE(logger->addSink(sink));

    logger->log(namica::LogLevel::Info, "hidden");
    logger->log(namica::LogLevel::Warning, "visible={}", namica::Int{1});
    sink->setLogMinLevel(namica::LogLevel::Error);
    logger->log(namica::LogLevel::Warning, "hidden after update");
    logger->log(namica::LogLevel::Error, "visible={}", namica::Int{2});
    logger->flush();

    EXPECT_EQ(readBinaryFile(path), "Gameplay:Warning|visible=1\nGameplay:Error|visible=2\n");
}

// 两个 Logger 共享同一 FileSink，移除第一个注册并释放局部持有者后，第二个仍应能写入。
// 预期已移除的 Logger 不再输出，剩余 Logger 的刷新包含此前全部有效记录；
// 只有最后一个 Logger 释放后对象才销毁、文件资源才回收，不能因一次注销提前关闭共享文件。
TEST_F(NamicaRuntimeTest, FileSinkRemainsUsableUntilLastLoggerReleasesIt)
{
    TemporaryDirectory directory{};
    auto const path{directory.path() / "shared.log"};
    auto config{makeFileConfig(path)};
    config.logFmt = "{2}|{3}";
    config.flushOnWrite = namica::False;
    auto first{namica::Logger::create("First")};
    auto second{namica::Logger::create("Second")};
    // 用 WeakPtr 观察生存期，避免测试自己的观察句柄延长 FileSink 的生命周期。
    namica::WeakPtr<namica::FileSink> lifetime{};
    {
        auto sink{namica::FileSink::create(config)};
        lifetime = sink;
        ASSERT_TRUE(first->addSink(sink));
        ASSERT_TRUE(second->addSink(sink));
        first->log(namica::LogLevel::Info, "before removal");
        ASSERT_TRUE(first->removeSink(sink));
    }
    ASSERT_FALSE(lifetime.expired());
    // 此时局部 sink 已释放，只剩第二个 Logger 持有；第一个的再次输出应被忽略。
    first->log(namica::LogLevel::Info, "not registered");
    first.reset();
    second->log(namica::LogLevel::Info, "still alive");
    second->flush();
    EXPECT_EQ(readBinaryFile(path), "First|before removal\nSecond|still alive\n");

    second.reset();

    EXPECT_TRUE(lifetime.expired());
    EXPECT_TRUE(std::filesystem::remove(path));
}
