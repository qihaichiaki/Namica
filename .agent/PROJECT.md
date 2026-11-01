# 当前项目事实

核对日期：2026-10-31（Asia/Shanghai）。核对时 HEAD 为 `2c983b5`（整理 core、app 目录结构）；用户已提交目录迁移和初始知识库，当前新增的 Log.h、Logger.h 尚未跟踪。这是快照，后续回答具体问题应检查对应文件。

2026-11-01 局部更新：本轮重新读取 Log.h、Logger.h、Types.h 及工作区状态；更新下列相关条目，没有重新验证其他模块或执行构建。

## 目标与背景

用户希望理解并亲自逐步构建游戏引擎。长期目标是接近成熟引擎组织方式的个人实用引擎；近期目标是一个足够简单、覆盖编辑器到打包发布全流程的小游戏。

当前 [项目日记](../doc/life/梦醒再梦.md) 的 26/10/31 条目明确启动第一阶段：完成贪吃蛇小游戏的编辑器操作、简化打包、独立发布流程。具体编辑能力与验收范围仍需逐步明确。历史横板 2D 设想、3D 渲染实验、agent 提过的收集与避障游戏，不覆盖这一更新的项目目标。

## 实际源码入口

| 位置 | 已观察的内容 |
| --- | --- |
| [根 CMakeLists.txt](../CMakeLists.txt) | C++20，加入 vendor、runtime、editor、tests；设置源码 assets 目录和构建输出 |
| [runtime/CMakeLists.txt](../runtime/CMakeLists.txt) | `Namica` 为 SHARED 库；公开 `include`，私有 `src`；私有链接 stb_image 和 nlohmann_json |
| [core/Base.h](../runtime/include/namica/core/Base.h) | Windows 平台判断、DLL 导入导出宏；用户已迁入 core，并新增 CLASS_DISABLE_COPY_MOVE 宏 |
| [core/Types.h](../runtime/include/namica/core/Types.h) | 原 Common.h；基础数值类型别名、String、布尔常量、`NAMICA_TOL`；2026-11-01 已有映射到 std 的 UniquePtr / SharedPtr / WeakPtr 模板别名 |
| [core/Log.h](../runtime/include/namica/core/Log.h) | 用户新建的空头文件，只有 pragma once 与 namica 命名空间；没有接口或实现 |
| [core/Logger.h](../runtime/include/namica/core/Logger.h) | 2026-11-01：Logger final，私有构造，禁拷贝/移动；声明析构、返回 SharedPtr 的两个 create 重载、setLogFmt / setDefaultLogFmt，成员 m_logFmt；尚无等级、过滤或消息输出接口，runtime 中未找到对应实现文件 |
| [app/Engine.h](../runtime/include/namica/app/Engine.h) | 用户已迁入 app，对应实现为 [src/app/Engine.cpp](../runtime/src/app/Engine.cpp)；仍是简单单例入口，尚未成为完整的系统生命周期管理器 |
| [math](../runtime/include/namica/math) | Vector、Mat4、Quat、Constants、角度转换等；采用 `.h + .inl` |
| [FileSystem.h](../runtime/include/namica/io/FileSystem.h) | 文件读取、资源根目录、程序目录查询、图片加载目前集中在一个类 |
| [FileSystem.cpp](../runtime/src/io/FileSystem.cpp) | 文件流读取、Windows 程序路径查询、stb_image 图片解码 |
| [editor/src/Main.cpp](../editor/src/Main.cpp) | 当前只等待约三秒并退出；没有可用编辑器 UI |
| [TestRender.h](../tests/playground/window_render/TestRender.h) | 窗口辅助函数、Texture、ShaderProgram、Material、VertexLayout、Transform、Camera、PlayerController、Mesh、Object、Light |
| [TestRender.cpp](../tests/playground/window_render/TestRender.cpp) | 实验性窗口与渲染实现，包含材质 JSON 读取和 glTF 模型加载 |
| [贪吃蛇实验](../tests/playground/snake_game/TestSnakeGame.cpp) | 通过 GoogleTest 入口运行的实验；不是正式游戏工程 |
| [数学测试](../tests/runtime/math) | Matrix、Quaternion 的测试代码 |
| [项目总览](../doc/项目总览.md) | 规范、构建约定和总体规划；部分目录是未来蓝图 |

检查时没有正式的顶层 `tooling/`、`tools/`、`projects/` 实现。文档中出现这些目录不表示它们已经创建或能构建。

2026-10-31 日志规划核对：用户已创建空的 Log.h 及 Logger.h 类骨架，尚无可用日志接口或实现；vendor 的 CMake 配置未接入 spdlog 或 fmt。窗口、Shader、纹理与模型加载实验使用分散的 `std::cout` / `std::cerr` 输出，见 TestRender.cpp。FileSystem 的部分失败路径直接返回空容器。窗口和输入能力目前主要在 playground，Engine 仍只有简单单例，编辑器入口未变化。

## 构建现状

- 根目录要求 CMake 3.21，C++20。
- 当前支持 Windows；预设使用 MSVC `cl.exe`、Ninja、外部准备的 x64 编译环境。
- [CMakePresets.json](../CMakePresets.json) 提供 `x64-debug`、`x64-release` 配置预设。使用前需核对本机编译工具和环境，不承诺普通终端已具备全部条件。
- 现有命名：`NAMICA_RUNTIME = Namica`，`NAMICA_EDITOR = NamicaEditor`。
- `NAMICA_BUILD_TESTS` 默认开启，`BUILD_PLAYGROUND` 默认关闭。
- runtime 数学测试通过 GoogleTest / CTest 组织；playground 中有图形窗口和交互程序，不应把它们当成全部可以无人值守运行的单元测试。
- [vendor/CMakeLists.txt](../vendor/CMakeLists.txt) 收拢第三方配置：stb_image、nlohmann_json、GoogleTest、GLFW、glad、cgltf 等。
- [窗口渲染测试配置](../tests/playground/window_render/CMakeLists.txt) 通过 `NAMICA_ASSETS_DIR` 使用源码目录中的资源。正式发布应验证能脱离该开发路径运行。
- 本次建库及后续审阅仅静态阅读和文档检查，没有配置、编译、运行游戏或执行测试。旧 include 路径问题已在后续扫描中消失，但不能将路径检查表述为实际编译或测试通过。

## 2026-10-31 目录迁移审阅

用户已自行将 `Base.h` 移至 `core/Base.h`、`Common.h` 改名为 `core/Types.h`、Engine 头与实现移至 app。初次审阅发现 FileSystem、数学头文件和 TestRender 中合计 7 处旧 include 引用；用户随后继续修复。

2026-10-31 在讨论 Log 时重新扫描 runtime、editor、tests，`namica/Base.h`、`namica/Common.h`、`namica/Engine.h` 的旧路径引用数量为 0。此项旧问题不应继续作为当前缺陷报告；扫描不等于完整构建验证，未运行编译器或测试。

## 仓库已有规范

来源：[doc/项目总览.md](../doc/项目总览.md) 与 [.clang-format](../.clang-format)。

- 文件夹小写加下划线，普通文件名大驼峰。
- 类名大驼峰，函数和方法小驼峰；形参带 `_` 前缀，成员等使用 `m_`、`s_` 等前缀；宏使用全大写加下划线。
- 格式遵循 `.clang-format`，不要为了自己的习惯进行批量格式改动。
- 第三方 CMake 配置集中在 vendor；构建产物与最终发布产物区分。
- 实际命名和格式存在历史代码，讨论规范不等于获准修正所有历史文件。

## 知识库与项目文档的关系

- `doc/` 是用户原有的学习与项目资料；`.agent/` 是接续指导用的上下文。
- `doc/项目总览.md` 已写入 runtime、editor、tooling 分开的蓝图，但 runtime 注释仍残留“引擎能力与内容处理工具库”等旧措辞；不能据此重新把 tooling 并回 runtime。
- 顶层 `serialization`、大量未来模块也是规划内容；最近的建议已收敛粒度，见 [ARCHITECTURE.md](ARCHITECTURE.md)。不要将早期蓝图当作必须逐项创建的文件清单。
- 2026-10-31 后续核对时，五个 `.agent/` 知识库文件已被用户纳入版本管理；根目录不再保留 AGENTS.md。agent 本次只更新知识库，不提交、不改 `.gitignore`，保留用户日记与 Log.h 等同期改动。

## 当前待继续的工作

2026-11-01：用户已在 Types.h 添加标准智能指针别名，在 Logger.h 声明返回 SharedPtr 的创建入口；不能再沿用“没有别名/没有创建声明”的旧快照，也不能据此认定所有对象都采用共享所有权。当前问题为日志等级过滤，用户强调先说明长期结构，当前只实现其子集。尚未观察到内存池或 GC 实现；日志输出、过滤及后端仍未实现或未定案。建议见 ARCHITECTURE，agent 仍只指导、审阅与维护知识库。
