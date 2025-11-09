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
            // NAMICA_CORE_WARN("set working dir: {}", ec.message().c_str());
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        NAMICA_CORE_WARN("当前设置的工作目录为空, 默认为当前进程的工作目录{0}",
                         getWorkingDirectory());
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