#include "runtime/log/utils/FileTestUtils.h"

#include <namica/core/Types.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <exception>
#include <fstream>
#include <random>
#include <system_error>

namespace
{

// 清理属于用例的一部分，失败需体现在测试结果中；报告过程自身也不能破坏 noexcept 析构。
void reportCleanupFailure(std::filesystem::path const& _path,
                          namica::Char const* _reason,
                          std::error_code const& _error = {}) noexcept
{
    try
    {
        ADD_FAILURE() << "Temporary directory cleanup failed: " << _path << ": " << _reason << " ("
                      << _error.message() << ")";
    }
    catch (...)
    {
        // GoogleTest 开启 throw_on_failure 时也不能让清理异常逃出析构函数。
        std::fputs("Temporary directory cleanup failed; failure reporting threw.\n", stderr);
    }
}

// 标准流没有可靠的逐次系统错误码，保留失败阶段和路径，使用通用流错误而不猜测 errno。
[[noreturn]] void throwFileFailure(namica::Char const* _reason, std::filesystem::path const& _path)
{
    throw std::filesystem::filesystem_error{
        _reason, _path, std::make_error_code(std::io_errc::stream)};
}

}  // namespace

namespace namica::test::log
{

// 先解析系统临时根目录，后续创建和清理始终使用同一个绝对根路径。
// 随机名称用于降低碰撞概率，真正的唯一性和所有权由 create_directory 的成功结果决定。
TemporaryDirectory::TemporaryDirectory()
    : m_root{std::filesystem::canonical(std::filesystem::temp_directory_path())}, m_path{}
{
    std::random_device random{};
    for (namica::Int attempt{0}; attempt < 128; ++attempt)
    {
        this->m_path = this->m_root /
            (namica::String{"namica-test-"} + std::to_string(random()) + "-" +
             std::to_string(random()) + "-" + std::to_string(attempt));
        std::error_code error{};
        // 原子创建成功后才完成构造；名称碰撞时重试，不能先 exists 再创建而引入进程间竞争。
        if (std::filesystem::create_directory(this->m_path, error))
        {
            return;
        }
        // 权限等真实环境错误直接上报，不把它们当作名称碰撞重试，也不接管已存在的目录。
        if (error && error != std::errc::file_exists)
        {
            throw std::filesystem::filesystem_error{
                "Failed to create temporary test directory", this->m_path, error};
        }
    }
    throw std::filesystem::filesystem_error{"Failed to reserve a unique temporary test directory",
                                            this->m_root,
                                            std::make_error_code(std::errc::file_exists)};
}

// 仅成功完成构造的对象会进入析构，因此 m_path 对应本对象实际创建的目录。
// 清理前验证归属和解析结果；发现目录被替换时宁可报告失败，也不递归删除可疑目标。
TemporaryDirectory::~TemporaryDirectory() noexcept
{
    try
    {
        // 只清理构造成功时独占的直属子目录，拒绝越界路径或符号链接替换。
        if (!this->m_root.is_absolute() || !this->m_path.is_absolute() ||
            this->m_path == this->m_root || this->m_path.parent_path() != this->m_root)
        {
            reportCleanupFailure(this->m_path, "Path is outside the expected temporary root");
            return;
        }

        std::error_code error{};
        auto const status{std::filesystem::symlink_status(this->m_path, error)};
        // 用例可能已经主动删除目录，此时没有剩余清理工作；其他检查失败仍应报告。
        if (error == std::errc::no_such_file_or_directory ||
            (!error && !std::filesystem::exists(status)))
        {
            return;
        }
        if (error)
        {
            reportCleanupFailure(this->m_path, "Failed to inspect directory", error);
            return;
        }
        if (!std::filesystem::is_directory(status) || std::filesystem::is_symlink(status))
        {
            reportCleanupFailure(this->m_path, "Owned directory was replaced");
            return;
        }

        // 再次解析可发现路径链中的符号链接变化；测试不得与清理并发替换目录树。
        auto const resolvedPath{std::filesystem::canonical(this->m_path, error)};
        if (error || resolvedPath != this->m_path)
        {
            reportCleanupFailure(
                this->m_path, "Directory no longer resolves to its owned path", error);
            return;
        }

        std::filesystem::remove_all(this->m_path, error);
        if (error)
        {
            reportCleanupFailure(this->m_path, "Failed to remove directory", error);
        }
    }
    catch (std::exception const& error)
    {
        reportCleanupFailure(this->m_path, error.what());
    }
    catch (...)
    {
        reportCleanupFailure(this->m_path, "Unexpected cleanup failure");
    }
}

std::filesystem::path const& TemporaryDirectory::path() const noexcept
{
    return this->m_path;
}

namica::String readBinaryFile(std::filesystem::path const& _path)
{
    std::ifstream file{_path, std::ios::in | std::ios::binary};
    if (!file.is_open() || file.fail())
    {
        throwFileFailure("Failed to open file for binary reading", _path);
    }

    namica::String content{};
    std::array<namica::Char, 4096> buffer{};
    while (file)
    {
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        // 最后一块可能不足缓冲大小；按 gcount 追加，既不丢尾部也不把 NUL 当字符串结束。
        content.append(buffer.data(), static_cast<namica::String::size_type>(file.gcount()));
    }
    // 正常读到 EOF 才视为成功，其他失败不能伪装成空文件或不完整的日志内容。
    if (file.bad() || !file.eof())
    {
        throwFileFailure("Failed to read binary file", _path);
    }
    return content;
}

void writeBinaryFile(std::filesystem::path const& _path, namica::String const& _content)
{
    std::ofstream file{_path, std::ios::out | std::ios::binary | std::ios::trunc};
    if (!file.is_open() || file.fail())
    {
        throwFileFailure("Failed to open file for binary writing", _path);
    }

    // 固定小块避免整个 String 长度转换为 streamsize 时溢出，同时按显式长度保留 NUL。
    constexpr namica::String::size_type chunkSize{4096};
    for (namica::String::size_type offset{0}; offset < _content.size();)
    {
        auto const count{std::min(chunkSize, _content.size() - offset)};
        file.write(_content.data() + offset, static_cast<std::streamsize>(count));
        if (file.fail())
        {
            throwFileFailure("Failed to write binary file", _path);
        }
        offset += count;
    }

    // close 也可能因最终刷新失败而设置错误状态，不能只检查 write。
    file.close();
    if (file.fail())
    {
        throwFileFailure("Failed to close binary file after writing", _path);
    }
}

}  // namespace namica::test::log
