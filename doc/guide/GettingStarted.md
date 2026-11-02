# 构建与接入

## 准备环境

当前使用 Windows x64、MSVC、Ninja 和 CMake 3.21 或更新版本。MSVC 需要支持 C++20 的 `<format>` 与 `<chrono>` 时区功能；仓库依赖使用 Git 子模块。

请使用已初始化 MSVC x64 环境的开发者终端。预设不会自动准备编译器环境；如果找不到 `cl.exe`，先检查终端环境。

## 构建和运行测试

克隆仓库后，在 `Namica/` 根目录执行：

```shell
git submodule update --init --recursive
cmake --preset x64-debug -DNAMICA_BUILD_TESTS=ON -DBUILD_PLAYGROUND=OFF
cmake --build build/x64-debug
ctest --test-dir build/x64-debug --output-on-failure
```

程序与运行所需 DLL 位于 `build/x64-debug/bin/`，库与链接产物位于 `build/x64-debug/lib/`。需要 Release 时，将命令中的预设和目录换为 `x64-release`。

常用选项：

| 选项 | 作用 |
| --- | --- |
| `NAMICA_BUILD_TESTS=ON` | 构建 runtime 测试；当前默认开启 |
| `BUILD_PLAYGROUND=ON` | 构建交互实验；需要同时开启 `NAMICA_BUILD_TESTS` |

只查看日志测试时，可以构建 `test_core` 后使用 `ctest --test-dir build/x64-debug -R Logger --output-on-failure`。图形和游戏实验需单独启动，见[应用入口与示例](Examples.md)。

## 在 C++ 目标中使用 Namica

当前通过源码工程中的 `Namica` CMake 目标接入运行库。例如，已有一个 `Main.cpp`，在同一工程中、`Namica` 目标定义之后添加应用目标：

```cmake
add_executable(MyApp Main.cpp)
target_link_libraries(MyApp PRIVATE Namica)
enable_project_options(MyApp)
```

`MyApp` 与 `Main.cpp` 是示例名称。这段配置使用仓库已有的 `enable_project_options`；链接 `Namica` 会传递公开头文件目录与 C++20 要求。使用者无需定义 `NAMICA_BUILD_DLL`。

代码通过 `<namica/...>` 包含公开头文件，例如 `<namica/core/Logger.h>`。各模块的完整示例可以作为 `Main.cpp`；标注为片段的代码放入对应函数中使用。

```shell
cmake --build build/x64-debug --target MyApp
./build/x64-debug/bin/MyApp.exe
```

运行时需能找到 `Namica.dll`；在仓库默认输出布局中，它和应用程序位于同一 `bin/` 目录。资源文件需按应用实际读取路径准备，详见[文件与图片](FileSystem.md)。

## 常见情况

- 配置时提示缺少 GoogleTest 或 GLFW：检查子模块是否已初始化。
- 运行 `NamicaEditor` 后没有界面：当前入口只等待约三秒后退出，尚无可用编辑器 UI。
- 示例找不到资源：可执行文件目录、进程工作目录和资源根目录是不同概念；先检查实际传入的路径。
- 构建 Release 不会自动整理资源或生成完整游戏发布包。

配置入口：[CMakePresets.json](../../CMakePresets.json) · [runtime/CMakeLists.txt](../../runtime/CMakeLists.txt)。返回[用户指南](README.md)。
