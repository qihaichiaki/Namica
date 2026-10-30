## glfw API学习

### glfwInit
```C++
    int glfwInit(void);
```

* 用来初始化GLFW库自身，为后续的窗口创建、输入事件处理、显示器查询和计时等功能准备运行环境
* 调用glfw函数开始之前需要调用的函数, 理解为glfw库的入口(初始化)函数, 并且后续函数需要在其所在的主线程中进行调用

### glfwWindowHint
```C++
    void glfwWindowHint(int hint, int value);
```

* 用来设置后续创建窗口时的配置，包括窗口属性、帧缓冲格式，以及OpenGL上下文的要求
* 用来设置OpenGL上下文要求的hint设置(要求满足对应opengl条件的上下文)
    - GLFW_CONTEXT_VERSION_MAJOR 请求的OpenGL主版本号
    - GLFW_CONTEXT_VERSION_MINOR 请求的OpenGL次版本号
    - GLFW_OPENGL_PROFILE 请求的OpenGL配置
        * GLFW_OPENGL_CORE_PROFILE opengl核心配置(现代化, 会舍弃一些接口)
        * GLFW_OPENGL_COMPAT_PROFILE opengl兼容配置
        * GLFW_OPENGL_ANY_PROFILE 不要求特定配置, 让创建过程按平台与实现的规则选择
* 其他常用的hint配置
    - GLFW_RESIZABLE 是否允许用户调整普通窗口大小(普通窗口指窗口化窗口, 和全屏窗口进行区分)
    - GLFW_VISIBLE 创建时候是否先隐藏普通窗口
    - GLFW_DECORATED 创建没有标题栏、边框等装饰的普通窗口
    - GLFW_SAMPLES 请求具有x个采样的MSAA帧缓冲(MSAA和抗锯齿相关)
    - GLFW_DEPTH_BITS 请求x位深度缓冲
    - GLFW_STENCIL_BITS 请求x位模板缓冲
    - GLFW_CLIENT_API 创建窗口时不创建OpenGL/OpenGLES上下文(GLFW_NO_API/GLFW_YES_API) (ES是是面向移动设备和嵌入式设备的OpenGL版本)
* 配置全部恢复为默认: glfwDefaultWindowHints
* 注意配置会一直保留, 创建窗口时会以当前配置为准, 后续修改通过此接口修改配置不会影响之前已创建窗口


### glfwGetPrimaryMonitor
```C++
    GLFWmonitor* glfwGetPrimaryMonitor(void);
```

* 获取主显示器，并返回代表它的GLFWmonitor*句柄

### glfwGetVideoMode
```C++
    const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* handle);
```

* 查询指定显示器当前正在使用的显示模式，包括宽高、RGB 颜色通道的位深，以及刷新率。
* GLFWvidmode -> 显示模式 struct字段(显示器当前采用的一组显示设置)
    - width 显示模式的宽度，单位为屏幕坐标
    - height 显示模式的高度，单位为屏幕坐标
    - redBits 红色通道的位深
    - greenBits 绿色通道的位深
    - blueBits 蓝色通道的位深
    - refreshRate 显示模式的刷新率，单位Hz

### glfwCreateWindow
```C++
    GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
```

* 通过设置的窗口提示(glfwWindowHint)，创建一个窗口；使用 OpenGL / OpenGL ES 时，还会同时**创建与这个窗口关联的图形上下文**。
* title 窗口标题，使用UTF-8编码
* monitor 全屏窗口使用的显示器；传nullptr创建窗口化窗口
* share 与哪个已有窗口的上下文共享资源；传nullptr表示不共享
    * 多个OpenGL上下文可以访问同一组可共享的OpenGL对象；每个上下文仍然保留自己的绑定、渲染状态，以及不能共享的对象。
    * 常见可以共享的对象: Texture, Buffer(VBO, EBO等), Shader对象, Program对象, Renderbuffer(RBO, 可作为帧缓冲附件的图像存储), Sampler对象(过滤、环绕等采样参数), Sync对象(用于协调GPU命令执行的同步对象)
    * 不可共享的对象:  VAO, FBO, Query对象

### glfwGetWindowSize
```C++
    void glfwGetWindowSize(GLFWwindow* handle, int* width, int* height);
```

* 获取指定窗口当前内容区域的宽度和高度，单位是**屏幕坐标**（screen coordinates），不包含标题栏和窗口边框.

### glfwSetWindowPos
```C++
    void glfwSetWindowPos(GLFWwindow* handle, int xpos, int ypos);
```

* 设置窗口在桌面上的位置。(窗口内容区域左上角在虚拟桌面中的坐标)
* 单位为屏幕坐标, 相对于虚拟桌面的原点的位移
* 如果存在多显示器, 则原点有可能不为(0, 0), 可通过glfwGetMonitorPos获取虚拟桌面位置

### glfwSetWindowAttrib
```C++
    void glfwSetWindowAttrib(GLFWwindow* handle, int attrib, int value);
```

* 在窗口创建之后，修改窗口的某些属性
* 支持修改如下的属性
    - GLFW_RESIZABLE 允许用户调整窗口大小
    - GLFW_DECORATED 显示系统提供的标题栏、边框等装饰
    - GLFW_FLOATING 让窗口位于普通非置顶窗口之上
    - GLFW_AUTO_ICONIFY 全屏窗口失去输入焦点时，自动最小化并恢复之前的显示模式(VideoMode)
    - GLFW_FOCUS_ON_SHOW 调用 glfwShowWindow显示窗口时，尝试获取输入焦点
    - GLFW_MOUSE_PASSTHROUGH 让鼠标输入穿过窗口，传递给后面的窗口(鼠标穿透, 只对无装饰窗口(就是去掉系统提供的标题栏、边框等外框元素，保留程序内容区域的窗口)有效)

### glfwMakeContextCurrent
```C++
    void glfwMakeContextCurrent(GLFWwindow* handle);
```

* 把window关联的 OpenGL / OpenGL ES 上下文，设为“调用这个函数的线程”的当前上下文。之后，这个线程发出的OpenGL调用就使用该上下文
* 传入nullptr表示解除当前线程与之前上下文的绑定


### glfwGetProcAddress
```C++
    typedef void (*GLFWglproc)(void);
    GLFWglproc glfwGetProcAddress(const char* procname);
```

* 根据函数名，查询当前OpenGL上下文中对应函数的入口地址
* 通常在glad中, 配合``gladLoadGLLoader``把glfwGetProcAddress当作回调，批量查询并保存OpenGL函数地址，方便后续直接使用 gl* 函数。
```C++
typedef void* (* GLADloadproc)(const char *name);

glfwMakeContextCurrent(window);
gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
```

* 为什么需要查询? 使用现代OpenGL时，许多函数入口需要在运行时获取。窗口系统、OpenGL 实现以及当前上下文决定了哪些功能可用；GLFW 提供统一的查询接口，封装底层平台的差异。
* ``gladLoadGLLoader``返回非0, 表示加载器加载成功


### glfwSetKeyCallback
```C++
    typedef void (* GLFWkeyfun)(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWkeyfun glfwSetKeyCallback(GLFWwindow* handle, GLFWkeyfun cbfun)
```

* 给指定窗口注册键盘事件回调函数
* 键盘按键事件: 按下, 重复, 松开

* 回调函数的glfwkeyFun参数解释
| 参数 | 含义 | 例子 |
|---|---|---|
| `window` | 收到这次事件的窗口 | 可以判断事件来自哪个窗口 |
| `key` | GLFW 定义的按键编号 | `GLFW_KEY_W`、`GLFW_KEY_ESCAPE` |
| `scancode` | 平台相关的按键扫描码 | 处理一些没有 GLFW 按键编号的键时有用 |
| `action` | 此次事件的动作 | 按下(GLFW_PRESS)、松开(GLFW_RELEASE)、重复(GLFW_REPEAT) |
| `mods` | 事件发生时的修饰键状态，使用位掩码表示 | Ctrl、Shift、Alt 等 |


### glfwWindowShouldClose
```C++
    int glfwWindowShouldClose(GLFWwindow* handle);
```

* 用于查询指定窗口的“关闭标志”
* 通常为用户设置或者通过glfwSetWindowShouldClose进行设置

### glfwPollEvents
```C++
    void glfwPollEvents(void);
```

* 处理当前已经到达的窗口和输入事件
* 多窗口系统中同样每个循环只需要调用一次
* 只能在主线程中使用

### glfwSwapBuffers
```C++
    void glfwSwapBuffers(GLFWwindow* handle);
```

* 交换指定窗口的前后缓冲，将这一帧的绘制结果提交用于显示
* 交换缓冲的时机可以通过`glfwSwapInterval`控制

### glfwTerminate
```C++
    void glfwTerminate(void);
```

* 结束 GLFW 的使用，销毁它管理的剩余窗口等对象，并释放 GLFW 分配的资源