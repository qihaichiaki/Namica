#pragma once

#include "namica/core/Base.h"
#include <string>

namespace Namica
{

class NAMICA_API FileSystem
{
public:
    /**
     * @brief 设置文件系统的工作目录
     *
     * @param _workingDirectory 当前文件系统的工作目录
     * @return 设置目录是否成功
     */
    static bool setWorkingDirectory(std::string_view _workingDirectory) noexcept;

    /**
     * @brief 获取文件系统的工作目录
     */
    static std::string getWorkingDirectory();

    /**
     * @brief 判断传入的路径是否是文件夹
     *
     * @param _directoryPath 文件夹目录
     */
    static bool isDirectory(std::string_view _directoryPath) noexcept;

    /**
     * @brief 判断传入的路径是否存在
     *
     * @param _filePath 路径
     */
    static bool exists(std::string_view _filePath) noexcept;
};

}  // namespace Namica