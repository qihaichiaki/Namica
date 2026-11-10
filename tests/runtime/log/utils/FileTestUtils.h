#pragma once

#include <namica/core/Types.h>

#include <filesystem>

namespace namica::test::log
{

// 为日志模块内的文件测试创建独占临时目录，隔离不同用例及 CTest 并行进程的文件。
// 系统临时目录须可写；测试期间不要替换本目录或并发修改其目录树。
// 使用其中路径的文件对象应先销毁，再由本对象检查归属并递归清理；清理失败报告给 GoogleTest，
// 不从析构函数抛异常。禁止复制移动，避免多个对象承担同一目录的清理职责。
class TemporaryDirectory final
{
public:
    TemporaryDirectory();
    ~TemporaryDirectory() noexcept;

    TemporaryDirectory(TemporaryDirectory const&) = delete;
    TemporaryDirectory(TemporaryDirectory&&) = delete;
    TemporaryDirectory& operator=(TemporaryDirectory const&) = delete;
    TemporaryDirectory& operator=(TemporaryDirectory&&) = delete;

    // 返回独占目录的绝对路径引用；引用有效期仅限本对象存活期间。
    [[nodiscard]] std::filesystem::path const& path() const noexcept;

private:
    std::filesystem::path m_root;
    std::filesystem::path m_path;
};

// 完整读取测试文件的原始字节，不转换编码、换行或 NUL；调用端先刷新或关闭待验证的写入端。
// 空文件返回空 String，打开或读取失败抛异常，不能将环境错误误判为“没有日志输出”。
[[nodiscard]] namica::String readBinaryFile(std::filesystem::path const& _path);

// 准备已有文件的初始内容：覆盖写入原始字节，空内容也会创建/清空文件，不自动创建父目录。
// 调用端须提供已存在的父目录；写入及最终关闭均检查错误，成功返回后可立即交给 FileSink 打开。
void writeBinaryFile(std::filesystem::path const& _path, namica::String const& _content);

}  // namespace namica::test::log
