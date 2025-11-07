## Namica
Namica当前规划为一个2d游戏引擎, 能够提供2d游戏的基本需求

### v0.0.1 任务规划
- [x] 目录结构确定
- [ ] 基本框架搭建完成
  - [x] Ref结构
  - [x] 日志系统
  - [ ] 内存管理
- [ ] 渲染API搭建完成
- [ ] 编辑器GUI框架完成
- [ ] ECS搭建完成
- [ ] box2d物理引擎引进
- [ ] build runtime系统


### 开发环境
当前仅仅只是在windows下使用msvc编译器使用C++17标准进行开发

#### vscode中使用clangd
* .vscode中settings.json包含如下:
```json
{
    "C_Cpp.intelliSenseEngine": "disabled",
    "clangd.path": "<path>/clangd.exe",
    "cmake.configureArgs": [
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    ],
    "clangd.arguments":[
        "--compile-commands-dir=${workspaceFolder}/<build...>",
        "--header-insertion=never"
    ],
}
```


* 注意:
  * DCMAKE_EXPORT_COMPILE_COMMANDS需要cmake构建器的支持, 如果是直接使用vs2022则COMMANDS.json无法生效
  * --header-insertion选项让clangd不自己自动推断引入什么头文件, 防止出现一些头文件的误入


### 开发模块
#### 通用头文件
* Base.h 编译接口/平台层宏控制, 简单轻便
* TypeMacros.h 类型语义/代码便捷宏
* Macros.h 功能宏

#### Ref结构
* 当前性能开销, 需求不明确, 关于智能指针的使用, 优先统一使用:
  * Ref使用std::shader_ptr替代
  * WeakRef使用std::weak_ptr
* 放在Memory.h 头文件中, 表示内存管理相关

#### Time
* 方便获取当前的时间戳，或者当地时区的格式化时间
* 后续增加各种Time方面的工具函数

#### 日志系统
* 当前项目使用C++20开发, 使用std::format可以更方便的输出自定义的信息，所以决定当前阶段简单封装其作为日志系统, 后续存在更高的性能要求或者需求无法实现在考虑三方库方案 -> Logger
* 引擎使用管理日志的地方: Log. 统一管理一个core logger和client logger, 其中core用于引擎内部输出日志, client用于外界的application输出

#### Application
* 应用程序的使用, 每个构建成exe的项目都只能同时存在一个Application