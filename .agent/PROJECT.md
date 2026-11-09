# 当前项目事实

日志、SystemTimePoint 与 Application 按 2026-11-09 当前工作区核对。记录、TextSink 文本布局与控制台输出已接通；以下区分源码事实和实际验证，不以编辑器运行成功代替全部测试通过。

## 实现与入口

| 范围 | 当前状态与位置 |
| --- | --- |
| runtime | [构建配置](../runtime/CMakeLists.txt) 生成 `Namica` DLL；公开 include，私有 src，模板/内联实现随头文件提供 |
| 基础声明 | [Base.h](../runtime/include/namica/core/Base.h) 提供 Windows 平台判断、DLL 导入导出及类约束宏；其他平台目前直接报编译错误 |
| 类型 | [Types.h](../runtime/include/namica/core/Types.h) 提供基本类型与标准智能指针别名；`UniquePtr<T>` 当前只暴露 T 参数 |
| 时间 | [TimePoint.h](../runtime/include/namica/core/TimePoint.h) 的 SystemTimePoint 构造时采集系统时间，getStr 按系统时区输出秒精度文本；该方法已为 const |
| Logger | [Logger.h](../runtime/include/namica/log/Logger.h)、[Logger.inl](../runtime/include/namica/log/Logger.inl)、[Logger.cpp](../runtime/src/log/Logger.cpp)：正文格式化、单条记录构建、逐端过滤派发与统一 flush 已实现，不再保存历史记录容器 |
| 日志记录 | [LogRecord.h](../runtime/include/namica/log/LogRecord.h) 拥有 SystemTimePoint、LogLevel、source、message；[LogLevel.h](../runtime/include/namica/log/LogLevel.h) 独立定义六级枚举 |
| 输出接口 | [LogSink.h](../runtime/include/namica/log/sink/LogSink.h) 提供等级访问、返回 Bool 的虚 write/flush |
| 文本输出 | [TextSink.h](../runtime/include/namica/log/sink/TextSink.h)、[TextSink.cpp](../runtime/src/log/sink/TextSink.cpp)：保存 logFmt/dateFmt，setLogFmt/setDateFmt 校验并返回 Bool，formatRecord(record) 为 const 并返回不含颜色/换行的文本行；构造校验失败向外传播异常，构造函数与析构已导出 |
| 控制台 | [ConsoleSink.h](../runtime/include/namica/log/sink/ConsoleSink.h) 继承 TextSink，实现标准流选择、整条等级配色、自动换行与每条刷新；Auto 已接入 Windows 终端/VT 检测 |
| Log | [Log.h](../runtime/include/namica/log/Log.h)、[Log.cpp](../runtime/src/log/Log.cpp) 保留 Core/App 的持有、借用入口和宏；私有 init 按 [LogConfig](../runtime/include/namica/log/LogConfig.h) 创建独立 Core/App 控制台输出；clear 刷新并释放持有者 |
| 应用 | [Application.h](../runtime/include/namica/app/Application.h)、[实现](../runtime/src/app/Application.cpp) 提供骨架与静态 get；构造拒绝已有实例，在 Log 初始化和启动诊断完成后登记 this，析构记录日志、清空实例指针并调用 Log::clear；[ApplicationConfig](../runtime/include/namica/app/ApplicationConfig.h) 包含 name 与 logConfig，当前只消费日志参数，不保存完整 Config，name 暂未使用；[编辑器入口](../editor/src/Main.cpp) 创建 Application 并调用 run，尚无主循环与窗口 |
| 文件图片 | [FileSystem.h](../runtime/include/namica/io/FileSystem.h)、[实现](../runtime/src/io/FileSystem.cpp) 提供路径查询、文件与图片读取 |
| 数学 | [math](../runtime/include/namica/math/) 提供 Vector、Mat4、Quat、常量及角度转换，主要采用 .h + .inl |
| 实验 | [窗口渲染](../tests/playground/window_render/)与[贪吃蛇](../tests/playground/snake_game/TestSnakeGame.cpp) 尚非完整正式运行模块或游戏工程 |
| 测试 | [runtime 测试](../tests/runtime/) 包含 log/math；[TestLog.cpp](../tests/runtime/log/TestLog.cpp) 有 19 项 Logger/Log 测试，[TestConsoleSink.cpp](../tests/runtime/log/TestConsoleSink.cpp) 有 14 项控制台测试，共 33 项，归入 test_log |

顶层 tooling 尚未实现。对外用法见 [用户指南](../doc/guide/README.md)；项目蓝图中出现的模块不等于已创建或必须立即实现。

## 影响后续开发的行为

- Logger 私有构造、create(name) 返回 SharedPtr，默认名称 NamicaLogger；log 先创建记录，仅在首个接收端出现时填写来源与格式化正文，随后向所有匹配端复用记录。正文格式化失败由 catch (...) 直接 break，当前没有失败统计或诊断；source 的 String 赋值仍位于该 try 外。log/flush 目前忽略 sink 的 Bool 返回值，错误收集由用户明确留待后续。addSink 拒绝空值与重复对象，removeSink 解除注册。
- LogSinkConfig 现在只含 minLevel，已导出的 protected 基类构造初始化最低等级。TextSinkConfig 增加 logFmt/dateFmt 和便利构造函数；ConsoleSinkConfig 继承它，包含 target、ConsoleColorTheme 与 Auto/Always/Never，logFmt/dateFmt 不再重复声明。
- TextSink 构造时校验日期/布局并抛 std::format_error；ConsoleSink 构造选择 cout/cerr 并应用颜色。ConsoleSink::write 调用 formatRecord 后整条着色、追加换行并每条 flush；直接调用 write 不自行过滤。
- setLogFmt/setDateFmt 由 TextSink 提供，校验成功才替换成员，无效返回 False 保留旧值；formatRecord 使用记录原时间、当前来源和正文。日期校验也依赖系统时区，当前 helper 捕获全部 std::exception 并返回 False。ConsoleSink::write 返回自身 flush 的结果；flush 调用 ostream::flush 后返回 !fail()，可反映写入或刷新留下的失败状态，但未保证格式化或流异常不逃逸。颜色枚举现已使用与 36m 对应的 Cyan。
- LogConfig 通过 haveCoreConsoleSink/haveAppConsoleSink 启停默认端，用 coreConsoleSinkConfig/appConsoleSinkConfig 分别配置；coreLoggerSource/appLoggerSource 默认 Core/App 并用于 LogRecord.source；默认控制台布局为 `[{0}][{1}]{2}: {3}`，来源由记录字段提供，不再写死 Core/App 文本。init 成功创建并注册两个局部 Logger 后才发布静态成员，已有实例则直接返回。clear 先刷新两个 Logger 再清空静态 SharedPtr；Application 正常析构已调用 clear。
- Log getter 返回借用 Logger&，有效期受 Application 初始化与清理限制；不支持初始化前或销毁后访问。失败路径仍需明确：初始化发布后的启动诊断若抛异常，不会进入 Application 析构；清理时任一 flush 抛异常会中断后续清理。
- 尚无 FileSink、编辑器接收实现或线程安全承诺；Windows 仍使用窄字符标准流，没有 UTF-8 到 UTF-16 的原生控制台写入路径。
- 未定义 NAMICA_LOG_DEBUG 时，四个 Trace/Debug 宏展开为 (void)0，日志参数不进入 C++ 表达式检查，也不被求值。直接调用 Logger::log 不受该开关影响。当前用 #ifdef，定义成 0 仍会启用。
- Assert 由 NAMICA_ENABLE_ASSERTS 控制；启用分支使用不带末尾分号的 do/while，失败会记录 Error 并在 Windows 调用 __debugbreak；关闭时展开为 (void)0，不求值条件。不能当作始终生效的唯一实例或输入校验机制。
- 构建开关由各目标独立设置：runtime 当前以 PRIVATE 启用 NAMICA_LOG_DEBUG 和 NAMICA_ENABLE_ASSERTS；editor 未定义二者。链接 DLL 不传播私有定义，editor 需要时可自行启用，只控制其自身编译的调用；这是已确认策略。
- 窗口、Shader 和资源诊断在 TestRender.cpp 中使用分散的 cout/cerr；GLFW 错误回调尚未注册。资源解析与业务失败恢复仍需调用层处理，日志不能代替它们。
- FileSystem 默认资源根为可执行文件目录；设置时只检查路径存在。文件失败和空文件都可能得到空结果，图片失败返回空容器；图片内存用匹配的 stbi_image_free 释放。
- Mat4 默认是零矩阵，变换向右乘；求逆在行列式恰为零时保持原值。Quat::angleAxis 要求单位轴，fromEuler 对应 qZ*qY*qX；极大数值的向量归一化仍有溢出边界。

## 当前边界与后续事项

以下按 2026-11-09 当前源码及已完成的测试核对：

- **错误收集（已明确后续实现）**：Sink 已返回 Bool，Logger 尚未消费；正文格式失败也仍静默丢弃。当前没有统计、限频后备诊断或完整逐端异常隔离，不将这些未来工作列为本轮必须补齐的功能。
- **生命周期边界**：来源赋值、sink 内日期/布局格式化和输出仍可抛异常，Application 析构中的日志和 clear 可能因此终止进程。初始化发布后的失败回滚、析构不抛和尽力清理所有端仍待完善，Bool 返回值本身不解决这些问题。
- **来源布局（已落地）**：TextSink 统一四参数 `{0}` 时间、`{1}` 等级、`{2}` 来源、`{3}` 正文；ConsoleSink 默认布局为 `[{0}][{1}]{2}: {3}`，Core/App 通过 LogRecord.source 表达；FileSink 尚未接入。
- **其他边界**：真实控制台编码尚未适配；公开等级/颜色枚举索引没有越界保护，需约定合法枚举前提或在输入处校验。平台私有辅助函数可放匿名命名空间；warringColor 命名仍可统一为 warningColor。

## 构建与源码约定

- [根配置](../CMakeLists.txt) 要求 CMake 3.21、C++20；当前为 Windows / MSVC / Ninja，预设依赖调用方准备 x64 工具链环境。
- [预设](../CMakePresets.json) 为 x64-debug、x64-release，输出到对应 build 子目录；NAMICA_BUILD_TESTS 默认 ON，BUILD_PLAYGROUND 默认 OFF。
- [vendor](../vendor/CMakeLists.txt) 集中接入 stb_image、nlohmann_json、GoogleTest、GLFW、glad、cgltf。
- 测试目标按模块目录自动生成，包括 test_log、test_math；交互实验为 test_snake_game 和 test_window_render，不能当作普通无人值守测试。渲染实验通过开发期资源路径访问 assets，尚无完整发布验证。
- 文件夹小写加下划线，普通文件/类名大驼峰，方法小驼峰、形参加下划线、成员使用前缀；遵循 [.clang-format](../.clang-format) 和用户当前编码约定，不批量修正无关历史代码。

构建、接入和实验运行命令维护在 [GettingStarted](../doc/guide/GettingStarted.md) 与 [Examples](../doc/guide/Examples.md)，不在本页重复。

## 最近实际验证

**2026-11-09：TextSink 抽象与四参数布局落地后，Debug / MSVC / Ninja 构建 Namica、test_log、NamicaEditor 成功；直接运行 test_log，33/33 通过。** 测试已按当前布局更新，覆盖 `{2}` 来源与 `{3}` 正文；运行库其余实现、CMake 脚本及依赖未修改。

覆盖各端等级过滤、无接收端/全拒收不格式化、多端接收只格式化一次、正文及来源所有权、注册/移除/共享、统一刷新；ConsoleSink 覆盖四参数布局（来源/正文）与日期构造校验、setter 失败保留旧值、配置所有权、stdout/stderr、UTF-8 文本、换行刷新与写入/刷新失败的 Bool 返回；Log 经 Application 验证 Core/App 配置及正常关闭时刷新释放。测试自定义 Sink 通过 DLL 公开接口构造和析构。

ConsoleSink 测试采用 Never 模式和受控 streambuf，失败缓冲不会操作真实标准句柄；这些结果不证明真实控制台中文、Auto/Always 颜色或平台适配正常。不测试尚未实现的 Logger 错误收集、异常恢复或关闭后重建策略。本轮未运行 Release、数学/图形回归或交互实验。
