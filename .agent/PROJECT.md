# 当前项目事实

核对日期：2026-11-01（Asia/Shanghai）。本次 Logger 审阅时 HEAD 为 `8a4e593`（Log 框架初步搭建），用户未提交的代码改动位于 Logger.cpp 和 TestLog.cpp。早期目录整理快照的 HEAD 为 `2c983b5`。以下信息有核对范围，后续仍需检查对应文件。

2026-11-01 最新局部更新：用户已完善 setLogFmt 的显式失败返回、移除 nodiscard/noexcept，并授权补充相关测试。新增 4 项普通用例后 Logger 共 17 项全部通过，仍统一位于 core/TestLog.cpp。Agent 没有修改 Logger 或其他引擎实现，也未改测试构建接入；其他模块未做完整回归。

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
| [core/Logger.h](../runtime/include/namica/core/Logger.h) | 2026-11-01：定义六级日志；Logger final，私有构造，禁拷贝/移动，create 返回 SharedPtr；setLogFmt 返回 Bool，当前无 nodiscard/noexcept；setLogMinLevel、模板 log、私有导出 println，成员 m_logFmt / m_minLogLevel |
| [core/Logger.inl](../runtime/include/namica/core/Logger.inl) | 用户已先判断最低等级，并将 make_format_args 改为直接借用命名参数 _args...；整数右值和临时字符串调用已通过编译及输出断言 |
| [src/core/Logger.cpp](../runtime/src/core/Logger.cpp) | 已实现受控创建、配置、当前时间、等级文本、ANSI 颜色及同步 std::cout 输出；setLogFmt 用三个 String 样本校验，格式错误返回 False 并保留旧布局，成功才赋值；消息提交阶段仍无格式错误兜底或输出目标扩展 |
| [core/Time.h](../runtime/include/namica/core/Time.h) | getCurrentTime 及日期/时间格式宏，实现位于 src/core/Time.cpp，使用 chrono 时区和 std::vformat |
| [TestLog.cpp](../tests/runtime/core/TestLog.cpp) | 当前 17 项普通测试全部通过；原有 13 项加 4 项布局测试，覆盖 6 种有效布局、10 种无效布局及状态保持、失败后继续配置、调用方字符串生命周期；旧用例也断言布局设置成功 |
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
- 2026-10-31 建库及早期审阅仅静态阅读和文档检查。2026-11-01 Logger 审阅运行了下述定向验证；未执行完整工程回归或图形程序，不能把定向测试通过表述为引擎整体验证通过。

## 2026-11-01 Logger 定向审阅与验证

- 使用现有 build/local_playground_debug 配置和 MSVC 14.51.36231，增量构建 test_core 返回无需更新；执行 NamicaRuntimeTest.logger：1 个用例通过，观察到仅 Info / Warning / Error / Critical 输出。该用例没有断言，因此通过不证明过滤及格式化契约完整。
- 系统临时目录中的独立探针引用现有头文件、链接现有 Namica.dll，不修改仓库源码或测试：log(Info, "value={}", 42) 与传入临时 String 均在 Logger.inl:12 报 C2664；对应 int 左值调用编译并正常输出。
- 在最低等级为 Info 时调用 log(Debug, "bad {") 仍抛 std::format_error，确认入口过滤过晚。setLogFmt("{3}") 被接受，下一次 Error 日志抛 std::format_error，确认格式配置未在设置时拒绝、输出时也无兜底。
- 管道捕获的现有用例输出含 ANSI 转义序列；runtime/editor/tests 未查到 GetConsoleMode / SetConsoleMode 或 VT 启用代码。颜色依赖终端环境，未验证全部 Windows 终端。
- 以上为初次审阅结果；后续过滤问题已由用户修复，最新验证见下一节。具体指导及当日官方参考见 ARCHITECTURE。未运行 Release、完整 CTest、多线程测试或图形程序。

## 2026-11-01 用户授权的 Logger 单元测试

- 授权范围：补充当前 Logger 已有功能的测试，由用户修正实现。最初新增过 tests/compile/logger 下的两个 .cpp 和独立目标；现按用户偏好移除，全部用例归入 tests/runtime/core/TestLog.cpp，测试 CMake 恢复普通发现流程。不修改 Logger、Time 或其他 runtime 实现。
- 写测试前已同步长期接口判断：create / log / setLogMinLevel 的职责可保留；setLogFmt 只验证现有默认输出上的有效布局字段，不要求未来所有 sink 共用布局。未锁定默认布局、ANSI 配色、线程模型、无效布局应抛何种异常或采用何种回退。
- TestLog.cpp 将原来只有输出的 smoke 用例升级为行为测试，合并后共 13 项；用 RAII 恢复 cout 捕获，比较文本时剥离 ANSI 颜色，时间只按显式布局中约定的字段检查形状。运行期错误格式测试使用动态 String，不妨碍未来对格式字面量进行编译期检查。
- 用户随后要求 runtime 测试采用 Types.h 类型；Logger 测试与配套编译检查已显式包含 Types.h，并使用 Int / Double / Char / String，整数右值检查改为 Int{42}，保留原有回归目标。
- 原 LoggerCompile.IntegerLiteral / TemporaryString 已被 LoggerFormatsIntegerRvalues / LoggerFormatsTemporaryStrings 两个普通用例替代，既要求合法调用能编译，也检查最终输出。没有禁用用例或将编译失败标记为预期成功。
- 首次补齐测试时，在 Debug / MSVC 14.51.36231 下共 13 项：9 通过、4 失败；包含两项过滤顺序测试和两项右值编译检查。
- 用户随后将最低等级判断移到 Logger::log 首次格式化之前，并移除 println 中的重复判断。再次增量构建 test_core（无需更新）并执行 CTest -R Logger：11 项运行期测试全部通过，2 项 LoggerCompile 仍失败，诊断为 Logger.inl:14 的 C2664。过滤问题已修复，不能继续报告为当前缺陷；11 项通过不包含两项独立编译检查。本次复核没有修改源码或测试。
- 组织调整时的验证：独立目标已移除，合并后的 test_core 最初仍因右值参数绑定而编译失败；当时没有运行旧二进制冒充当前结果。
- 前次复核：用户将 make_format_args(std::forward<Args>(_args)...) 改为 make_format_args(_args...) 后，test_core 构建成功、13 项普通用例全部通过，包括两个右值用例。当时未覆盖无效布局设置和被接收消息的格式错误恢复。
- 最新补测：用户明确表示“已完善，可以补充相关单测”。核对 setLogFmt 已返回 Bool、移除 nodiscard/noexcept 后，增加 LoggerAcceptsValidLayouts、LoggerRejectsInvalidLayoutsWithoutChangingState、LoggerChangesLayoutAfterRejectedUpdate、LoggerOwnsConfiguredLayout；沿用 Types.h 类型及中文注释。6 种有效布局覆盖省略/重复字段、转义、对齐/精度、纯文本和空串；10 种无效布局覆盖括号、字段越界、三个 String 字段的错误类型、索引混用与动态宽度类型错误。失败返回 False 且不抛格式异常，后续输出仍使用旧布局及等级阈值；随后可更新为新布局，调用方字符串修改/销毁不影响已保存配置。原有有效设置也改为 ASSERT_TRUE，避免准备阶段失败被后续输出掩盖。
- 最新验证：现有 Debug / MSVC 环境重新编译 Logger.cpp、TestLog.cpp 并链接 test_core 成功；CTest -R Logger 显示 17/17 通过、0 失败（约 0.43 秒）。只修改测试和知识库，未改引擎实现或 CMake。测试约束公开结果与输出，不调用私有校验函数、不固化其试格式化实现或 nodiscard 属性。普通消息被接收后格式错误的恢复策略仍未定案，未添加对应测试。
- 复跑需在已初始化 MSVC 环境的开发者终端先构建 test_core，再运行 `ctest --test-dir D:/dev/Namica/build/local_playground_debug -R Logger --output-on-failure`。完整用例输出见构建目录中的 Testing/Temporary/LastTest.log。
- 未运行完整数学/图形测试、Release 或多线程检查；没有为了让测试变绿而修改实现。设置无效布局延迟报错的问题已由用户修复，新增失败行为测试已通过；不能据此宣称消息格式错误恢复或整个引擎已完成验证。

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

2026-11-01：用户已完善 setLogFmt，当前声明为 NAMICA_API Bool setLogFmt(String const&)，无 nodiscard/noexcept；无效布局返回 False 且保留旧配置。用户授权补测后，新增 4 项、合计 17 项 Logger 普通测试已全部通过。当前测试范围已包含布局失败反馈、状态保持、恢复设置及输入字符串生命周期；消息提交的格式失败策略仍待讨论。后续继续按用户当前问题指导，不因测试通过自动扩展或改写日志实现。
