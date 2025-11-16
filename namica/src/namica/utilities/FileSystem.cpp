#include "namica/utilities/FileSystem.h"
#include "namica/core/Log.h"
#include <filesystem>
#include <fstream>

namespace Namica
{

bool FileSystem::setWorkingDirectory(std::string_view _workingDirectory) noexcept
{
    if (!_workingDirectory.empty())
    {
        std::error_code ec;
        std::filesystem::current_path(_workingDirectory, ec);
        if (ec)
        {
            NAMICA_CORE_WARN("设置工作目录出现错误: {0}", ec.message());
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        NAMICA_CORE_WARN("当前设置的工作目录为空, 默认使用当前进程使用的目录");
        return false;
    }
}

std::string FileSystem::getWorkingDirectory()
{
    auto str = std::filesystem::current_path().generic_u8string();
    return {str.begin(), str.end()};
}

bool FileSystem::isDirectory(std::string_view _directoryPath) noexcept
{
    std::error_code ec;
    return std::filesystem::is_directory(_directoryPath, ec);
}

bool FileSystem::exists(std::string_view _filePath) noexcept
{
    std::error_code ec;
    return std::filesystem::exists(_filePath, ec);
}

bool FileSystem::createDirectories(std::string_view _dirPath) noexcept
{
    std::error_code ec;
    return std::filesystem::create_directories(_dirPath, ec);
}

std::string FileSystem::readFileText(std::string const& _filePath)
{
    std::ifstream ifs{_filePath, std::ios::in | std::ios::binary};
    if (ifs)
    {
        std::string source;
        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        if (size != 0)
        {
            source.resize(size);
            ifs.seekg(0, std::ios::beg);
            ifs.read(&source[0], source.size());
            return source;
        }
        else
        {
            NAMICA_CORE_WARN("未能从此文件中读到任何信息: {0}", _filePath);
        }
    }
    else
    {
        NAMICA_CORE_ERROR("未能找到此文件: {0}", _filePath);
        NAMICA_CORE_ASSERT(false);
    }

    return std::string{};
}

static uint64_t fnv1a(std::string const& _data)
{
    // fnv1a 实现
    uint64_t const FNV_PRIME = 1099511628211ull;
    uint64_t const OFFSET_BASIS = 1469598103934665603ull;

    uint64_t hash = OFFSET_BASIS;
    for (char c : _data)
    {
        hash ^= (uint64_t)c;
        hash *= FNV_PRIME;
    }
    return hash;
}

uint64_t FileSystem::readFileToHash(std::string const& _filePath)
{
    std::string source = readFileText(_filePath);
    if (source.empty())
    {
        return 0;
    }

    return fnv1a(source);
}

std::tuple<std::string, uint64_t> FileSystem::readFileAndHash(std::string const& _filePath)
{
    std::string source = readFileText(_filePath);
    if (source.empty())
    {
        return {source, 0};
    }

    return {source, fnv1a(source)};
}

}  // namespace Namica