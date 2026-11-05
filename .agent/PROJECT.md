# 当前项目事实

日志、SystemTimePoint 与 Application 按 2026-11-05 当前工作区核对；其余模块记录来自此前阅读。日志处于未完成迁移中，以下静态事实不代表构建通过；实际执行的验证单独注明。

## 实现与入口

| 范围 | 当前状态与位置 |
| --- | --- |
| runtime | [构建配置](../runtime/CMakeLists.txt) 生成 `Namica` DLL；公开 include，私有 src，模板/内联实现随头文件提供 |
| 基础声明 | [Base.h](../runtime/include/namica/core/Base.h) 提供 Windows 平台判断、DLL 导入导出及类约束宏；其他平台目前直接报编译错误 |
| 类型 | [Types.h](../runtime/include/namica/core/Types.h) 提供基本类型与标准智能指针别名；`UniquePtr<T>` 当前只暴露 T 参数 |
| 时间 | [TimePoint.h](../runtime/include/namica/core/TimePoint.h) 的 SystemTimePoint 构造时采集系统时间，getStr 按系统时区输出秒精度文本；该方法目前尚非 const |
| Logger | [Logger.h](../runtime/include/namica/log/Logger.h)、[Logger.inl](../runtime/include/namica/log/Logger.inl)、[Logger.cpp](../runtime/src/log/Logger.cpp)：正文格式化、记录容器与输出端注册已有；输出与 flush 尚未接通 |
| 日志记录 | [LogRecord.h](../runtime/include/namica/log/LogRecord.h) 拥有 SystemTimePoint、LogLevel、source、message；[LogLevel.h](../runtime/include/namica/log/LogLevel.h) 独立定义六级枚举 |
| 输出端 | [LogSink.h](../runtime/include/namica/log/sink/LogSink.h) 提供等级访问、虚 write/flush；[ConsoleSink.h](../runtime/include/namica/log/sink/ConsoleSink.h) 与配置已声明，实现只有 create，构造、布局设置和输出尚未补全 |
| Log | [Log.h](../runtime/include/namica/log/Log.h)、[Log.cpp](../runtime/src/log/Log.cpp) 保留 Core/App 的持有、借用入口和宏；私有 init 仍使用 [LogConfig](../runtime/include/namica/log/LogConfig.h) 的旧 Logger 配置接口，尚未适配输出端 |
| 应用 | [Application.h](../runtime/include/namica/app/Application.h)、[实现](../runtime/src/app/Application.cpp) 提供骨架与静态 get；构造拒绝已有实例，在 Log 初始化和启动诊断完成后登记 this，析构清空实例指针；[ApplicationConfig](../runtime/include/namica/app/ApplicationConfig.h) 包含 name 与 logConfig，当前只消费日志参数，不保存完整 Config，name 暂未使用；[编辑器入口](../editor/src/Main.cpp) 创建 Application 并调用 run，尚无主循环与窗口 |
| 文件图片 | [FileSystem.h](../runtime/include/namica/io/FileSystem.h)、[实现](../runtime/src/io/FileSystem.cpp) 提供路径查询、文件与图片读取 |
| 数学 | [math](../runtime/include/namica/math/) 提供 Vector、Mat4、Quat、常量及角度转换，主要采用 .h + .inl |
| 实验 | [窗口渲染](../tests/playground/window_render/)与[贪吃蛇](../tests/playground/snake_game/TestSnakeGame.cpp) 尚非完整正式运行模块或游戏工程 |
| 测试 | [runtime 测试](../tests/runtime/) 包含 core/math；[Logger 测试](../tests/runtime/core/TestLog.cpp) 有 17 项普通行为用例 |

顶层 tooling 尚未实现。对外用法见 [用户指南](../doc/guide/README.md)；项目蓝图中出现的模块不等于已创建或必须立即实现。

## 影响后续开发的行为

- Logger 私有构造、create 返回 SharedPtr；log 当前直接 std::vformat，再将记录追加至无容量限制的 m_logRecords，source 暂为占位文本。println 旧实现已注释，没有向已注册输出端分发，flush 为空。addSink 拒绝空指针与同实例重复注册，removeSink 按共享指针指向的对象删除注册。
- ConsoleSinkConfig 继承 LogSinkConfig，包含 logFmt 与按时间/等级/正文区分的颜色配置；LogSinkConfig 目前还包含 dateFormat。ConsoleSink 只有固定三项 m_logColorIndex，尚未实现布局、终端检测、编码适配、颜色恢复或刷新。
- LogSink 的 m_minLogLevel 当前没有默认初始化或基类构造保证；set/get 已实现，过滤执行位置尚未接通。没有 FileSink、编辑器接收实现或线程安全承诺。
- Log 私有持有两个 SharedPtr；导出的 getter 返回借用 Logger&，init 私有且由 Application 友元调用。LogConfig 仍包含 coreConfig/appConfig 的 logFmt/minLevel；init 调用的 Logger::setLogFmt/setLogMinLevel 已不在当前类声明中，测试也仍使用旧接口。
- init 代码仍是先准备局部持有者后赋静态成员；已有对象时复制 SharedPtr，不创建对象副本。新输出端接入后还需重新确定初始化、失败回滚和关闭协议。
- Log 的有效使用以 Application 完成日志初始化为前提，getter 没有空值保护；不要求在 Application 创建前可用。两个静态 SharedPtr 当前未随 Application 析构清空，具体关闭协议尚未实现，不能据此承诺应用销毁后的调用有效。
- 未定义 NAMICA_LOG_DEBUG 时，四个 Trace/Debug 宏展开为 (void)0，日志参数不进入 C++ 表达式检查，也不被求值。直接调用 Logger::log 不受该开关影响。当前用 #ifdef，定义成 0 仍会启用。
- Assert 由 NAMICA_ENABLE_ASSERTS 控制；启用分支使用不带末尾分号的 do/while，失败会记录 Error 并在 Windows 调用 __debugbreak；关闭时展开为 (void)0，不求值条件。不能当作始终生效的唯一实例或输入校验机制。
- 构建开关由各目标独立设置：runtime 当前以 PRIVATE 启用 NAMICA_LOG_DEBUG 和 NAMICA_ENABLE_ASSERTS；editor 未定义二者。链接 DLL 不传播私有定义，editor 需要时可自行启用，只控制其自身编译的调用；这是已确认策略。
- 窗口、Shader 和资源诊断在 TestRender.cpp 中使用分散的 cout/cerr；GLFW 错误回调尚未注册。资源解析与业务失败恢复仍需调用层处理，日志不能代替它们。
- FileSystem 默认资源根为可执行文件目录；设置时只检查路径存在。文件失败和空文件都可能得到空结果，图片失败返回空容器；图片内存用匹配的 stbi_image_free 释放。
- Mat4 默认是零矩阵，变换向右乘；求逆在行列式恰为零时保持原值。Quat::angleAxis 要求单位轴，fromEuler 对应 qZ*qY*qX；极大数值的向量归一化仍有溢出边界。

## 当前待修正项

以下为 2026-11-05 对进行中实现的静态审阅；不将未补全函数逐一视为独立缺陷，未改源码或新增测试：

- **结构边界**：Logger 的无界记录容器会混入历史存储职责；LogSinkConfig 的日期格式会强制结构化接收端携带文本策略。建议见 ARCHITECTURE 的 ConsoleSink 审阅。
- **接通前检查**：修正 LogSinkConfig 中 namica/Log 的大小写；为最低等级提供确定初值；SystemTimePoint::getStr 应支持 const LogRecord 的只读访问；迁移 Log::init、LogConfig 与测试中的旧接口。颜色枚举、等级映射与格式字段不应共用未定义语义的数字索引。
- **应用重建**：复用 SharedPtr 不产生配置事务，新输出端初始化仍需明确整体失败回滚与关闭后重建规则。

## 构建与源码约定

- [根配置](../CMakeLists.txt) 要求 CMake 3.21、C++20；当前为 Windows / MSVC / Ninja，预设依赖调用方准备 x64 工具链环境。
- [预设](../CMakePresets.json) 为 x64-debug、x64-release，输出到对应 build 子目录；NAMICA_BUILD_TESTS 默认 ON，BUILD_PLAYGROUND 默认 OFF。
- [vendor](../vendor/CMakeLists.txt) 集中接入 stb_image、nlohmann_json、GoogleTest、GLFW、glad、cgltf。
- 测试目标包括 test_core、test_math；交互实验为 test_snake_game 和 test_window_render，不能当作普通无人值守测试。渲染实验通过开发期资源路径访问 assets，尚无完整发布验证。
- 文件夹小写加下划线，普通文件/类名大驼峰，方法小驼峰、形参加下划线、成员使用前缀；遵循 [.clang-format](../.clang-format) 和用户当前编码约定，不批量修正无关历史代码。

构建、接入和实验运行命令维护在 [GettingStarted](../doc/guide/GettingStarted.md) 与 [Examples](../doc/guide/Examples.md)，不在本页重复。

## 最近实际验证

**2026-11-03：Debug / MSVC / Ninja 增量构建含首次初始化局部对象修复的 NamicaEditor、test_core 成功，Logger 定向测试 17/17 通过。** 当时编辑器默认启动返回 0，输出 Core 启动、run 与析构诊断。

上述结果对应迁移前源码。2026-11-05 仅静态阅读进行中的新接口，未运行构建或测试；不能将旧结果用于当前工作区。

覆盖等级边界、被过滤消息不格式化、右值与临时参数、UTF-8、有效布局、无效布局保持旧状态、失败后继续配置及配置字符串生命周期。测试保持在普通 core/TestLog.cpp，观察公开行为，不锁定颜色或私有实现。

这 17 项仍是独立 Logger 回归，不覆盖 LogConfig 的自定义启动配置、配置失败后的整体状态、重复 Application、宏裁剪或 Assert 展开；本轮未新增测试。结果不覆盖真实终端编码/颜色、完整数学/图形回归、Release、多线程、文件输出或消息格式失败恢复；本轮未运行交互实验。

复跑时需先配置并构建相应目标，再运行 Logger 定向测试；不能用旧二进制代表新源码。
