# 当前项目事实

Application、Log、Logger 按 2026-11-03 当前工作区核对；其余模块记录来自 2026-11-02。以下实现信息来自静态阅读，包含用户正在编写的内容；实际执行的验证单独注明，不因文档整理而视为重新测试。

## 实现与入口

| 范围 | 当前状态与位置 |
| --- | --- |
| runtime | [构建配置](../runtime/CMakeLists.txt) 生成 `Namica` DLL；公开 include，私有 src，模板/内联实现随头文件提供 |
| 基础声明 | [Base.h](../runtime/include/namica/core/Base.h) 提供 Windows 平台判断、DLL 导入导出及类约束宏；其他平台目前直接报编译错误 |
| 类型 | [Types.h](../runtime/include/namica/core/Types.h) 提供基本类型与标准智能指针别名；`UniquePtr<T>` 当前只暴露 T 参数 |
| 时间 | [Time.h](../runtime/include/namica/core/Time.h) 的 getCurrentTime 返回系统时区下秒精度的日历时间文本；没有帧计时或异常兜底 |
| Logger | [Logger.h](../runtime/include/namica/core/Logger.h)、[Logger.inl](../runtime/include/namica/core/Logger.inl)、[Logger.cpp](../runtime/src/core/Logger.cpp)：六级过滤、同步格式化、布局校验和控制台输出 |
| Log | [Log.h](../runtime/include/namica/core/Log.h)、[Log.cpp](../runtime/src/core/Log.cpp) 实现 Core/App 两个 Logger 的持有与借用访问、六级便捷宏及 Assert 宏；私有 init 由 Application 友元调用 |
| 应用 | [Application.h](../runtime/include/namica/app/Application.h)、[实现](../runtime/src/app/Application.cpp) 提供骨架与静态 get；构造拒绝已有实例并登记 this，析构清空实例指针；[ApplicationConfig](../runtime/include/namica/app/ApplicationConfig.h) 仅包含 name，构造保存配置并初始化 Log，run 与析构带诊断宏；[编辑器入口](../editor/src/Main.cpp) 创建 Application 并调用 run，尚无主循环与窗口 |
| 文件图片 | [FileSystem.h](../runtime/include/namica/io/FileSystem.h)、[实现](../runtime/src/io/FileSystem.cpp) 提供路径查询、文件与图片读取 |
| 数学 | [math](../runtime/include/namica/math/) 提供 Vector、Mat4、Quat、常量及角度转换，主要采用 .h + .inl |
| 实验 | [窗口渲染](../tests/playground/window_render/)与[贪吃蛇](../tests/playground/snake_game/TestSnakeGame.cpp) 尚非完整正式运行模块或游戏工程 |
| 测试 | [runtime 测试](../tests/runtime/) 包含 core/math；[Logger 测试](../tests/runtime/core/TestLog.cpp) 有 17 项普通行为用例 |

顶层 tooling 尚未实现。对外用法见 [用户指南](../doc/guide/README.md)；项目蓝图中出现的模块不等于已创建或必须立即实现。

## 影响后续开发的行为

- Logger 私有构造、`create()` 返回 SharedPtr；先过滤再使用 `std::vformat`。控制台输出含 ANSI 颜色，每条通过 `std::endl` 刷新。默认布局字段为时间、等级、正文。
- `setLogFmt` 使用三个 String 字段校验；成功返回 True，无效布局返回 False 且保持旧值。当前没有 nodiscard/noexcept；不据此推导全项目禁用这些修饰。
- Logger 尚无文件输出、结构化来源字段、订阅、消息提交失败恢复或线程安全承诺。未接入 spdlog，Log 已接入 Application 的启动、run 和析构诊断。
- Log 私有持有两个 SharedPtr；公开且导出的 getCoreLogger/getAppLogger 返回借用 Logger&。init 私有且不导出，Application 为友元；当前默认布局分别带 Core/App 文本前缀。Logger 的 private println 已导出，供公开 log 模板跨 DLL 调用；独立 Logger 的默认布局宏已移到 cpp 内部。
- Log 的有效使用以 Application 完成日志初始化为前提，getter 没有空值保护；不要求在 Application 创建前可用。两个静态 SharedPtr 当前未随 Application 析构清空，具体关闭协议尚未实现，不能据此承诺应用销毁后的调用有效。
- 未定义 NAMICA_LOG_DEBUG 时，四个 Trace/Debug 宏展开为 (void)0，日志参数不进入 C++ 表达式检查，也不被求值。直接调用 Logger::log 不受该开关影响。当前用 #ifdef，定义成 0 仍会启用。
- Assert 由 NAMICA_ENABLE_ASSERTS 控制；启用分支使用不带末尾分号的 do/while，失败会记录 Error 并在 Windows 调用 __debugbreak；关闭时展开为 (void)0，不求值条件。不能当作始终生效的唯一实例或输入校验机制。
- 构建开关由各目标独立设置：runtime 的 NAMICA_LOG_DEBUG 当前已注释，NAMICA_ENABLE_ASSERTS 为 PRIVATE；editor 当前未定义二者。链接 DLL 不传播私有定义，editor 需要时可自行启用，只控制其自身编译的调用；这是已确认策略。
- 窗口、Shader 和资源诊断在 TestRender.cpp 中使用分散的 cout/cerr；GLFW 错误回调尚未注册。资源解析与业务失败恢复仍需调用层处理，日志不能代替它们。
- FileSystem 默认资源根为可执行文件目录；设置时只检查路径存在。文件失败和空文件都可能得到空结果，图片失败返回空容器；图片内存用匹配的 stbi_image_free 释放。
- Mat4 默认是零矩阵，变换向右乘；求逆在行列式恰为零时保持原值。Quat::angleAxis 要求单位轴，fromEuler 对应 qZ*qY*qX；极大数值的向量归一化仍有溢出边界。

## 当前待修正项

以下为 2026-11-03 静态审阅发现，未改源码，也未添加对应测试：

- **初始化失败回滚**：Application 在 Log::init 前登记 s_instance；若后续初始化或启用时的启动日志抛异常，Application 本身的析构不会执行，实例指针会残留并阻止重试。Log::init 先发布 Core 再创建 App，也存在后者创建失败后的半初始化状态，需要与应用占用一起考虑失败清理。

## 构建与源码约定

- [根配置](../CMakeLists.txt) 要求 CMake 3.21、C++20；当前为 Windows / MSVC / Ninja，预设依赖调用方准备 x64 工具链环境。
- [预设](../CMakePresets.json) 为 x64-debug、x64-release，输出到对应 build 子目录；NAMICA_BUILD_TESTS 默认 ON，BUILD_PLAYGROUND 默认 OFF。
- [vendor](../vendor/CMakeLists.txt) 集中接入 stb_image、nlohmann_json、GoogleTest、GLFW、glad、cgltf。
- 测试目标包括 test_core、test_math；交互实验为 test_snake_game 和 test_window_render，不能当作普通无人值守测试。渲染实验通过开发期资源路径访问 assets，尚无完整发布验证。
- 文件夹小写加下划线，普通文件/类名大驼峰，方法小驼峰、形参加下划线、成员使用前缀；遵循 [.clang-format](../.clang-format) 和用户当前编码约定，不批量修正无关历史代码。

构建、接入和实验运行命令维护在 [GettingStarted](../doc/guide/GettingStarted.md) 与 [Examples](../doc/guide/Examples.md)，不在本页重复。

## 最近实际验证

**2026-11-03：Debug / MSVC / Ninja 增量构建当前 NamicaEditor、test_core 成功，Logger 定向测试 17/17 通过。** 运行当前 NamicaEditor 返回 0；当前关闭了 runtime 的 Trace/Debug 宏，启动、run 与析构没有日志输出。

覆盖等级边界、被过滤消息不格式化、右值与临时参数、UTF-8、有效布局、无效布局保持旧状态、失败后继续配置及配置字符串生命周期。测试保持在普通 core/TestLog.cpp，观察公开行为，不锁定颜色或私有实现。

这 17 项仍是独立 Logger 回归，不覆盖新 Log 宏、初始化前调用、重复 Application、Assert 展开或初始化失败；本轮未新增测试。结果也不覆盖完整数学/图形回归、Release、多线程、文件输出或消息格式失败恢复。指南示例与交互实验命令未在本轮单独编译运行。

复跑时需先配置并构建相应目标，再运行 Logger 定向测试；不能用旧二进制代表新源码。
