# 文件与图片

`FileSystem` 提供资源根目录设置、文件读取和图片解码。路径使用 `std::filesystem::path`。

## 先分清路径

| 调用 | 路径如何解释 |
| --- | --- |
| `loadFile / loadFileText / loadImage` | 直接使用传入路径；普通相对路径相对于进程工作目录 |
| `loadAssetFile / loadAssetFileText / loadAssetImage` | 将传入的相对路径拼到已配置的资源根目录下 |
| `getExecutableFolder()` | 查询可执行程序所在目录 |
| `getAssetsFolder()` | 查询当前资源根目录 |

默认资源根目录是可执行程序目录。通过 `setAssetsFolder` 可以更改；如果设置的是相对路径，后续解析仍受进程工作目录影响。传给 `loadAsset...` 的路径应是相对资源路径；绝对路径会覆盖前面的根路径。

## 读取文本

下面的完整示例要求从仓库根目录运行，读取仓库已有的 `assets/shader/base.vert`：

```cpp
#include <namica/io/FileSystem.h>
#include <namica/core/Types.h>
#include <filesystem>
#include <iostream>

int main()
{
    namica::FileSystem files{};
    if (!files.setAssetsFolder(std::filesystem::path{"assets"}))
    {
        return 1;
    }

    namica::String const text{files.loadAssetFileText("shader/base.vert")};
    if (text.empty())
    {
        return 1;
    }
    std::cout << text;
    return 0;
}
```

若资源放在程序旁边，可以使用 `files.getExecutableFolder() / "assets"` 设置资源根目录，从而不依赖启动时的工作目录。

读取二进制数据使用 `loadFile` 或 `loadAssetFile`，返回 `std::vector<namica::Char>`；文本版本返回字符串，不进行文本编码转换。

## 读取图片

以下片段接在上例的 `files` 初始化之后使用。假设资源目录中已有 `image/Texture.png`，请换成自己的图片路径：

```cpp
namica::Int width{};
namica::Int height{};
namica::Int channels{};
auto pixels{files.loadAssetImage("image/Texture.png", width, height, channels)};

if (!pixels.empty())
{
    std::cout << width << " x " << height << ", channels=" << channels << '\n';
}
```

返回值是拥有自身数据的 `std::vector<namica::UChar>`，每个通道占一个字节，按图片原有通道数解码。成功时通过三个输出参数取得宽、高和通道数；数据大小为 `width * height * channels`。调用者不需要手动释放解码内存，上传纹理等 GPU 操作由使用方完成。

## 失败如何判断

- 文件打开或读取失败返回空容器 / 空字符串；空文件也会得到空结果。若需要区分二者，当前接口没有单独的错误状态。上面的非空 Shader 示例将空结果当作未读到内容。
- 图片解码失败返回空容器，此时不要使用输出的宽、高和通道数。
- `setAssetsFolder` 目前只检查路径存在，不检查它是否为目录；传入不存在的路径返回 `False` 并保留旧设置。文件系统查询本身仍可能抛出异常。
- 当前只提供读取，没有文件写入、资源缓存或目录访问隔离功能。

公开接口：[FileSystem.h](../../runtime/include/namica/io/FileSystem.h)。返回[用户指南](README.md)。
