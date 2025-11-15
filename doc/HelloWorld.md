## Namica
Namica当前规划为一个2d游戏引擎, 能够提供2d游戏的基本需求

### v0.0.1 任务规划
- [x] 目录结构确定
- [ ] 基本框架搭建完成
  - [x] Ref结构
  - [x] 日志系统
  - [x] 事件系统
  - [x] Window
  - [x] Layer
- [ ] 渲染API搭建完成
  - [ ] Renderer2D
  - [ ] 资源管理器
- [ ] 编辑器GUI框架完成
- [ ] ECS搭建完成
- [ ] box2d物理引擎引进
- [ ] build runtime系统


### 开发环境
当前仅仅只是在windows下使用msvc编译器使用C++20标准进行开发

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

#### Time[Core]
* 方便获取当前的时间戳，或者当地时区的格式化时间
* 后续增加各种Time方面的工具函数

#### 日志系统[Core]
* 当前项目使用C++20开发, 使用std::format可以更方便的输出自定义的信息，所以决定当前阶段简单封装其作为日志系统, 后续存在更高的性能要求或者需求无法实现在考虑三方库方案 -> Logger
* 引擎使用管理日志的地方: Log. 统一管理一个core logger和client logger, 其中core用于引擎内部输出日志, client用于外界的application输出

#### 文件系统[Utilities]
* 设置工作目录, 方便后续文件工作均在此目录下使用

#### RendererContext[Renderer]
* 获取窗口上下文, 正确的初始化renderer相关, 方便后续的正常渲染工作

#### Event[Events]
* 事件贯穿整个Application, 存在事件交互才能让引擎的各个部件受到事件影响做出对应的改变
* 事件涉及的逻辑是保证事件的非退化, 即在事件类的继承体系中, 处理事件时直接拿到的就是其子类, 而无需通过获取字段类型进行ifelse判断
* 核心机制是EventDispatcher, 作为一个调度器, 其根据模板传入的Event事件类型选择当前的Event是否是对应的事件, 如果是方可执行

#### Window[Core]
* 窗口创建, 当前统一使用GLFW三方库作为窗口创建依赖
* 窗口所在的sdk(比如glfw)只初始化一次, 然后window可以多创建, 一个window对应一个RendererContext,RendererContext根据窗口局部初始化渲染相关的上下文, 准备开始后续的渲染
* 窗口创建完毕后, 需要处理当前设备的各种事件, 并且将事件分类规划好传递给Application进行后续的处理 

#### Layer[Core]
* Layer作为在Application内处理逻辑的最小作业单位
  * push -> onAttach, pop -> onDetach, onUpdate, onEvent, onImGuiRender, getLayerName
* Layer的生命周期由Application中管理, LayerStack用作存储Layer的使用顺序, 提供遍历/插入手段

#### Renderer[Renderer]
* 渲染体系大致由如下体系构成:
  * Renderer(外部能直接调用的完全(3d+2d)渲染) ------------------> RendererCommand(渲染命令) ---------------> RendererAPI(OpenGL, Vulkan...)
  *          |-> Renderer2D -|(2d特供渲染)
* Renderer系列接口均是静态调用，根据配置, 在Renderer初始化阶段选择好底层的渲染API
* Renderer初始化阶段, 基本是底层渲染API的初始化 + 特定场景渲染API的初始化, 比如Renderer2D的初始化(准备好复用的顶点数组，shader等)
* Renderer2D阶段
  * 2D开始场景渲染，传入相机的pv矩阵(glm数学库)
  * 开始渲染后, 一般开始收集系列矩形, 圆形，线段等图形以及Texture2D进行批量渲染
  * 结束场景渲染, 绘制当前批次的渲染(注意过程中可能存在渲染, 取决于批次渲染所设定的个数)

#### Application[Core]
* 应用程序的使用, 每个构建成exe的项目都只能同时存在一个Application

* Application init:
  * 先设置文件系统的工作目录, 后续相对路径均依赖此工作目录进行
  * 根据传入的窗口配置创建主window
  * window注入Application事件函数, 方便window捕获的事件传入到application
  * Renderer2D的初始化