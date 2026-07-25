#pragma once

#include "namica/Base.h"
#include "namica/Common.h"
#include <filesystem>
#include <vector>

namespace namica
{

/**
 * @brief 文件系统
 *
 * 默认以当前程序所在文件夹作为资源目录去寻找, 否则指定资源目录
 *
 */
class FileSystem
{
public:
    NAMICA_API FileSystem();
    NAMICA_API ~FileSystem();

    /**
     * @brief 设置资产文件夹的路径
     *
     * @param _path 资产文件夹路径
     * @return 是否设置成功
     */
    NAMICA_API Bool setAssetsFolder(std::filesystem::path const& _path);

    /**
     * @brief 获取当前设置的资产目录所在路径
     *
     * @return 资产目录所在路径
     */
    NAMICA_API std::filesystem::path getAssetsFolder() const;

    /**
     * @brief 返回当前可执行程序所在的文件夹路径
     *
     * @return 可执行程序所在文件夹路径
     */
    NAMICA_API std::filesystem::path getExecutableFolder() const;

    // 加载图片功能

    NAMICA_API std::vector<UChar> loadImage(std::filesystem::path const& _path,
                                            Int& _outWidth,
                                            Int& _outHeight,
                                            Int& _outChannels);
    NAMICA_API std::vector<UChar> loadAssetImage(std::filesystem::path const& _relativePath,
                                                 Int& _outWidth,
                                                 Int& _outHeight,
                                                 Int& _outChannels);

    // 加载通用文件功能

    NAMICA_API std::vector<Char> loadFile(std::filesystem::path const& _path);
    NAMICA_API std::vector<Char> loadAssetFile(std::filesystem::path const& _relativePath);

    NAMICA_API std::string loadFileText(std::filesystem::path const& _path);
    NAMICA_API std::string loadAssetFileText(std::filesystem::path const& _relativePath);

private:
    std::filesystem::path m_assetsFolderPath;
};

}  // namespace namica