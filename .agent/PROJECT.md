# 当前项目事实

实现核对：2026-11-02，HEAD 为 `bfd70b1`。以下实现信息来自静态阅读；实际执行的验证单独注明，不因文档整理而视为重新测试。

## 实现与入口

| 范围 | 当前状态与位置 |
| --- | --- |
| runtime | [构建配置](../runtime/CMakeLists.txt) 生成 `Namica` DLL；公开 include，私有 src，模板/内联实现随头文件提供 |
| 基础声明 | [Base.h](../runtime/include/namica/core/Base.h) 提供 Windows 平台判断、DLL 导入导出及类约束宏；其他平台目前直接报编译错误 |
| 类型 | [Types.h](../runtime/include/namica/core/Types.h) 提供基本类型与标准智能指针别名；`UniquePtr<T>` 当前只暴露 T 参数 |
| 时间 | [Time.h](../runtime/include/namica/core/Time.h) 的 getCurrentTime 返回系统时区下秒精度的日历时间文本；没有帧计时或异常兜底 |
| Logger | [Logger.h](../runtime/include/namica/core/Logger.h)、[Logger.inl](../runtime/include/namica/core/Logger.inl)、[Logger.cpp](../runtime/src/core/Logger.cpp)：六级过滤、同步格式化、布局校验和控制台输出 |
| Log | [Log.h](../runtime/include/namica/core/Log.h) 为空，尚无公共访问或生命周期接口 |
| 应用 | [Engine.cpp](../runtime/src/app/Engine.cpp) 返回函数局部静态实例的借用指针；[编辑器入口](../editor/src/Main.cpp) 只等待约三秒后退出 |
| 文件图片 | [FileSystem.h](../runtime/include/namica/io/FileSystem.h)、[实现](../runtime/src/io/FileSystem.cpp) 提供路径查询、文件与图片读取 |
| 数学 | [math](../runtime/include/namica/math/) 提供 Vector、Mat4、Quat、常量及角度转换，主要采用 .h + .inl |
| 实验 | [窗口渲染](../tests/playground/window_render/)与[贪吃蛇](../tests/playground/snake_game/TestSnakeGame.cpp) 尚非完整正式运行模块或游戏工程 |
| 测试 | [runtime 测试](../tests/runtime/) 包含 core/math；[Logger 测试](../tests/runtime/core/TestLog.cpp) 有 17 项普通行为用例 |

顶层 tooling 尚未实现。对外用法见 [用户指南](../doc/guide/README.md)；项目蓝图中出现的模块不等于已创建或必须立即实现。

## 影响后续开发的行为

- Logger 私有构造、`create()` 返回 SharedPtr；先过滤再使用 `std::vformat`。控制台输出含 ANSI 颜色，每条通过 `std::endl` 刷新。默认布局字段为时间、等级、正文。
- `setLogFmt` 使用三个 String 字段校验；成功返回 True，无效布局返回 False 且保持旧值。当前没有 nodiscard/noexcept；不据此推导全项目禁用这些修饰。
- Logger 尚无文件输出、来源字段、订阅、统一入口、消息提交失败恢复或线程安全承诺。未接入 spdlog，业务侧调用主要仍集中在测试。
- 窗口、Shader 和资源诊断在 TestRender.cpp 中使用分散的 cout/cerr；GLFW 错误回调尚未注册。资源解析与业务失败恢复仍需调用层处理，日志不能代替它们。
- FileSystem 默认资源根为可执行文件目录；设置时只检查路径存在。文件失败和空文件都可能得到空结果，图片失败返回空容器；图片内存用匹配的 stbi_image_free 释放。
- Mat4 默认是零矩阵，变换向右乘；求逆在行列式恰为零时保持原值。Quat::angleAxis 要求单位轴，fromEuler 对应 qZ*qY*qX；极大数值的向量归一化仍有溢出边界。

## 构建与源码约定

- [根配置](../CMakeLists.txt) 要求 CMake 3.21、C++20；当前为 Windows / MSVC / Ninja，预设依赖调用方准备 x64 工具链环境。
- [预设](../CMakePresets.json) 为 x64-debug、x64-release，输出到对应 build 子目录；NAMICA_BUILD_TESTS 默认 ON，BUILD_PLAYGROUND 默认 OFF。
- [vendor](../vendor/CMakeLists.txt) 集中接入 stb_image、nlohmann_json、GoogleTest、GLFW、glad、cgltf。
- 测试目标包括 test_core、test_math；交互实验为 test_snake_game 和 test_window_render，不能当作普通无人值守测试。渲染实验通过开发期资源路径访问 assets，尚无完整发布验证。
- 文件夹小写加下划线，普通文件/类名大驼峰，方法小驼峰、形参加下划线、成员使用前缀；遵循 [.clang-format](../.clang-format) 和用户当前编码约定，不批量修正无关历史代码。

构建、接入和实验运行命令维护在 [GettingStarted](../doc/guide/GettingStarted.md) 与 [Examples](../doc/guide/Examples.md)，不在本页重复。

## 最近实际验证

**2026-11-01：Debug / MSVC 构建 test_core 成功，Logger 定向测试 17/17 通过。**

覆盖等级边界、被过滤消息不格式化、右值与临时参数、UTF-8、有效布局、无效布局保持旧状态、失败后继续配置及配置字符串生命周期。测试保持在普通 core/TestLog.cpp，观察公开行为，不锁定颜色或私有实现。

该结果不覆盖完整数学/图形回归、Release、多线程、文件输出或消息格式失败恢复。2026-11-02 的源码与用户指南工作仅静态核对；指南示例和交互命令未编译运行。

复跑时需先配置并构建相应目标，再运行 Logger 定向测试；不能用旧二进制代表新源码。
