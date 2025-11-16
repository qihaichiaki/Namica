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

    /**
     * @brief 创建文件目录
     *
     * @param _dirPath 文件目录路径
     */
    static bool createDirectories(std::string_view _dirPath) noexcept;

    /**
     * @brief 读取文件内容到string中
     *
     * @param _filePath 文件路径
     * @return std::string 文件内容的缓冲区
     */
    static std::string readFileText(std::string const& _filePath);

    /**
     * @brief 读取一个文件计算其hash值
     *
     * @param _filePath 文件路径
     * @return uint64_t 文件哈希值, 方便判断相同文件是否发生了修改
     */
    static uint64_t readFileToHash(std::string const& _filePath);

    /**
     * @brief 读取一个文件内容并且获取其hash值
     *
     * @param _filePath 文件路径
     * @return std::tuple<std::string, uint64_t> 文件内容和文件hash值
     */
    static std::tuple<std::string, uint64_t> readFileAndHash(std::string const& _filePath);
};

}  // namespace Namica