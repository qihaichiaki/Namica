#include "namica/io/FileSystem.h"

#ifdef NAMICA_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <fstream>
#include <stb_image.h>

namespace namica
{

FileSystem::FileSystem() : m_assetsFolderPath{this->getExecutableFolder()}
{
}

FileSystem::~FileSystem() = default;

Bool FileSystem::setAssetsFolder(std::filesystem::path const& _path)
{
    if (std::filesystem::exists(_path))
    {
        m_assetsFolderPath = _path;
        return True;
    }

    return False;
}

std::filesystem::path FileSystem::getExecutableFolder() const
{
#ifdef NAMICA_PLATFORM_WINDOWS
    wchar_t buffer[MAX_PATH]{};
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path{buffer}.remove_filename();
#else
    return std::filesystem::path{};
#endif
}

std::filesystem::path FileSystem::getAssetsFolder() const
{
    return m_assetsFolderPath;
}

std::vector<UChar> FileSystem::loadImage(std::filesystem::path const& _path,
                                         Int& _outWidth,
                                         Int& _outHeight,
                                         Int& _outChannels)
{
    using StbiImagePtr = std::unique_ptr<unsigned char, decltype(&stbi_image_free)>;

    StbiImagePtr imageData{
        stbi_load(_path.generic_string().c_str(), &_outWidth, &_outHeight, &_outChannels, 0),
        &stbi_image_free};

    if (!imageData)
    {
        return {};
    }

    Int const size{_outWidth * _outHeight * _outChannels};
    return {imageData.get(), imageData.get() + size};
}

std::vector<UChar> FileSystem::loadAssetImage(std::filesystem::path const& _relativePath,
                                              Int& _outWidth,
                                              Int& _outHeight,
                                              Int& _outChannels)
{
    return this->loadImage(m_assetsFolderPath / _relativePath, _outWidth, _outHeight, _outChannels);
}

std::vector<Char> FileSystem::loadFile(std::filesystem::path const& _path)
{
    // 读文件流 二进制读取(按照文件中的字节原样读取, 不做任何处理) |
    // 打开文件后定位到文件末尾(可以直接得到文件大小)
    std::ifstream file{_path, std::ios::binary | std::ios::ate};

    if (!file.is_open())
    {
        return {};
    }

    auto size{file.tellg()};  // 文件大小
    file.seekg(0);            // 从文件开始开始读取

    std::vector<Char> buffer(size);

    if (!file.read(buffer.data(), size))
    {
        return {};
    }

    return buffer;
}

std::vector<Char> FileSystem::loadAssetFile(std::filesystem::path const& _relativePath)
{
    return this->loadFile(m_assetsFolderPath / _relativePath);
}

std::string FileSystem::loadFileText(std::filesystem::path const& _path)
{
    auto buffer{this->loadFile(_path)};
    return std::string{buffer.begin(), buffer.end()};
}

std::string FileSystem::loadAssetFileText(std::filesystem::path const& _relativePath)
{
    return this->loadFileText(m_assetsFolderPath / _relativePath);
}

}  // namespace namica