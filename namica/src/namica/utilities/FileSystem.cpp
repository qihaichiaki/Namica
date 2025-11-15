#include "namica/utilities/FileSystem.h"
#include "namica/core/Log.h"
#include <filesystem>

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

}  // namespace Namica