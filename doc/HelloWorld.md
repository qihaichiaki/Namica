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
  - [x] Renderer2D
  - [ ] 资源管理器
- [x] 编辑器GUI框架完成
  - [x] viewport
- [x] ECS搭建完成
  - [x] entity, component, Scene
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

#### VertexLayout[Renderer]
* 顶点布局, 方便将顶点缓冲区中每一个点的布局指明, 上传到渲染系统中正确读取

#### VertexBuffer[Renderer]
* 顶点缓冲区, 准备将所有顶点数据传给shader的缓冲区

#### IndexBuffer[Renderer]
* 依据每个顶点的索引进行渲染. 朝着面向进行逆时针旋转, OpenGL渲染中以三角形为准

#### VertexArray[Renderer]
* 渲染目标的综合对象, 结合顶点缓冲区, 索引缓冲区的对象

#### UniformBuffer[Renderer]
* 所有shader共享的统一变量缓冲区, 即不需要针对每种shader上传一次统一变量
* 通常使用在开始场景渲染时，传入相机的PV矩阵

#### Shader[Renderer]
* 顶点shader和片段shader: 顶点shader每个顶点运行一次, 片段shader每个像素运行一次

#### Texure2D[Renderer]
* 2D纹理对象, 可以用来加载实际的png, jpg图片

#### Camra[Renderer]
* 相机对象, 便于传入开始场景绘制时, 准备好相机的 `投影*视图矩阵`

#### FrameBuffer[Renderer]
* 帧缓冲区, 便于绘制内容不直接渲染到窗口上，而是渲染到一张纹理上

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
  * Renderer的初始化

#### Entity[Scene]
* ECS系统中的标识符, 需要做到最简, 使用entt::entity进行标识即可, 后续相关操作组件的方法直接使用此即可

#### Component[Scene]
* ECS系统的中的最重要的Component数据聚合,由各个不同的Component组成了形形色色的Entity对象, 用来表达创作者的意图
* 创建一个空对象, 其初始拥有的Component存在Tag(UUID, 唯一标识符, 标识此Entity), Transform. 这些组件是无法从Entity中主动移除的

#### Scene[Scene]
* 组件聚合处, 通过查询不同的组件，从而实现其对应的功能.
* 渲染模块: 查询全部的sprite组件, 获取其transform和相关texture进行渲染即可
* 物理模块: 查询到全部的刚体组件, 对transform执行物理规则约束即可(集成box2d)

#### Editor APP[Editor]
##### Editor GUI
* 当前设计的EditorGUI组成: Panels + Viewports
* Editor GUI主要组成部分示意图:
```
+----------------------------------------------------+
|                  Menu Bar                          |
+--------------+----------------------+--------------+
|              |      Viewport        |  Inspector   |
|  Hierarchy   |      SceneView       |  Properties  |
+  (Scene)     +----------------------+--------------+
|              |      Project Files/  |     ...      |
|              |         Console      |              |
+--------------+----------------------+--------------+
```

* 核心面板:
  * HierarchyPanel 场景树
  * InspectorPanel 组件编辑器
  * ProjectPanel   文件浏览器
  * ConsolePanel   日志输出
  * Viewport       场景视口
  * Toolbar        工具栏

* GUI框架整体设计思路:
* 面板类设计：提供基本的onImGuiRenderer方法用于渲染。layer中存在面板中可能需要共享的内容，各个面板使用此变量时即可同步状态
*layer运行阶段:
  1. 初始化阶段：初始化各个面板, 加载各种GUI所需要的资源。
  2. 运行阶段：
     - 更新阶段遍历每个面板的onupdate进行更新。(可能需要注意顺序，因为存在信号监听机制)
     - 渲染阶段首先启动imgui docking，随后遍历每个面板的onImGuiRenderer进行渲染更新。
