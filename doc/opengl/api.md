## OpenGL API学习

### glClearColor
```C++
void glClearColor(r, g, b, a);
```
* 设置清除*颜色缓冲区*的清除值
* 类似于`GL_COLOR_BUFFER_BIT`这样的颜色缓冲区, 里面存储着的是RGBA值

### glClear
```C++
void glClear(BIT);
// BIT:
// GL_COLOR_BUFFER_BIT   颜色缓冲区
// GL_DEPTH_BUFFER_BIT   深度缓冲区
// GL_STENCIL_BUFFER_BIT 模板缓冲区
```
* 清除当前绑定的对应类型帧缓冲区内的值
* 其中不同的类型需要用不同的方式设置清除值
    * GL_COLOR_BUFFER_BIT 的清除通过glClearColor进行设置
    * GL_DEPTH_BUFFER_BIT 的清除通过glClearDepth进行设置
    * GL_STENCIL_BUFFER_BIT 的清除通过glClearStencil进行设置
* 颜色缓冲区就是存储对应每个像素的颜色(窗口创建后默认情况下是窗口的后缓冲区)
* 深度缓冲区存储的是每个像素的深度信息(1.0 ~ 0.0, 1.0标识离摄像机的最远处? 一般用于标识哪个像素离得摄像机最近? 哪个像素该挡住后面的像素...)
* 模板缓冲区理解为遮罩(暂时未遇到)


### glGetString
```C++
const char* glGetString(GLenum);
// GLenum:
// GL_VENDOR   OpenGL的实现方
// GL_RENDERER 渲染的实际后端
// GL_VERSION  当前OpenGL的版本
```
* 用来查询一些OpenGL的上下文信息


### glCreateShader
```C++
Gluint glCreateShader(SHADER_TYPE);
// SHADER_TYPE:
//   GL_VERTEX_SHADER   顶点着色器
//   GL_FRAGMENT_SHADER 片段着色器
// return: SHADER_OBJ
```
* 创建着色器

### glShaderSource
```C++
void glShaderSource(SHADER_OBJ, SOURCE_COUNT, SOURCES, SOURCE_LENS);
// SHADER_OBJ: 通过glCreateShader创建出的shader对象
// SOURCE_COUNT 源码存在几段, 一般为一段(opengl允许你拆成多段，然后将其合并为一个源码处理)
// SOURCES const char*[] 源码字符串
// SOURCE_LENS int* 源码段数的各自长度, 可以设置为nullptr(要求:每个源码字符串都是普通 C 字符串，以 \0 结尾)
```

* 设置着色器源码

### glCompileShader
```C++
void glCompileShader(SHADER_OBJ);
```

* 编译着色器对象(理解为编译一个C++翻译单元cpp)
* vs和fs都需要进行编译(两个CPP)

### glGetShaderiv
```C++
void glGetShaderiv(SHADER_OBJ, SHADER_STATUS_TYPE, OUT_SHADER_STATUS);
// SHADER_STATUS_TYPE/OUT_SHADER_STATUS:
//   GL_COMPILE_STATUS 编译状态(OUT: 0失败, 1成功)
```

* 获取着色器对象中的对应类型的状态的值

### glGetShaderInfoLog
```C++
void glGetShaderInfoLog(SHADER_OBJ, BUFFER_SIZE, OUT_BUFFER_LENGTH, OUT_BUFFER);
// OUT_BUFFER_LENGTH 可传入null, 不接收缓冲区长度
```

* 获取着色器对象的日志信息(一般用于错误状态时的信息), 写入到缓冲区中

### glCreateProgram
```C++
Gluint glCreateProgram();
// return: PROGRAM_OBJ 着色器程序对象
```

* 创建程序, 用于附加所有的着色器对象

### glAttachShader
```C++
void glAttachShader(PROGRAM_OBJ, SHADER_OBJ);
```

* 为着色器程序附加着色器对象
* 着色器对象需要先进行编译(?未确认)

### glLinkProgram
```C++
void glLinkProgram(PROGRAM_OBJ);
```

* 将所有的着色器对象链接到程序中

### glGetProgramiv
```C++
void glGetShaderiv(PROGRAM_OBJ, PROGRAM_STATUS_TYPE, OUT_PROGRAM_STATUS);
// PROGRAM_OBJ shader程序对象
// PROGRAM_STATUS_TYPE/OUT_PROGRAM_STATUS: 程序查询状态
//    GL_LINK_STATUS: 程序link状态(0失败/1成功)
```

* 获取着色器程序对象中的对应类型的状态的值

### glGetProgramInfoLog
```C++
void glGetProgramInfoLog(PROGRAM_OBJ, BUFFER_SIZE, OUT_BUFFER_LENGTH, OUT_BUFFER);
// OUT_BUFFER_LENGTH 可传入null, 不接收缓冲区长度
```

* 获取着色器程序对象的日志信息(一般用于错误状态时的信息), 写入到缓冲区中

### glDeleteShader
```C++
void glDeleteShader(SHADER_OBJ);
```

* 从GPU中删除着色器对象(一般用于shader程序链接成功后删除冗余数据, 因为内部已经保存了链接好了的代码)

### glGenBuffers/glCreateBuffers
```C++
void glCreateBuffers(BUFFER_LEN, OUT_BUFFER_OBJS);
// BUFFER_LEN:      创建的缓冲区个数
// OUT_BUFFER_OBJS: 输出的缓冲区对象(GLuint*) 
```

* 创建缓冲区对象, 方便后续从CPU上传数据到GPU中

### glBindBuffer
```C++
void glBindBuffer(BUFFER_TYPE, BUFFER_OBJ);
// BUFFER_TYPE: 绑定缓冲区类型
//    GL_ARRAY_BUFFER:         顶点数据数组(vbo)
//    GL_ELEMENT_ARRAY_BUFFER: 索引数据数组(ebo)
```

* 将buffer object绑定到某个缓冲区类型, 为后续的上传相关类型数据做准备
* 如果BUFFER_OBJ设置为0, 则表示对应缓冲区类型取消buffer obj的绑定

### glBufferData
```C++
void glBufferData(BUFFER_TYPE, BUFFER_SIZE, BUFFER_DATA, DRAW_TYPE);
// BUFFER_TYPE: 缓冲区类型
// BUFFER_SIZE: 传输数据CPU端大小
// BUFFER_DATA: 缓冲区数据起始位置指针
// DRAW_TYPE:   缓冲区上传模式类型
//   GL_STATIC_DRAW   静态数组(一次性上传完毕, 后续不再上传->永久留在GPU内存中)
//   GL_DYNAMIC_DRAW  动态数组(多次上传, 非固定顶点数据)
```

* 上传buffer数据

### glGenVertexArrays/glCreateVertexArrays
```C++
void glCreateVertexArrays(VAO_LEN, OUT_VAOS);
// VAO_LEN:  创建的顶点数组对象个数
// OUT_VAOS: 输出的顶点数组对象(GLuint*) 
```

* 解释顶点数组中的数据, VAO(vertex array obj)
* 顶点数组对象中存储了顶点属性在vbo中的layout以及如何映射到shader输入的位置

### glBindVertexArray
```C++
void glBindVertexArray(VAO);
```

* 绑定当前的VAO
* 需要注意, 如果bind vertex Buffer, 一般规范是要在bind VAO再次进行绑定, 逻辑更加清晰
* GL_ELEMENT_ARRAY_BUFFER 必须在VAO之后再次绑定, 否则索引缓冲区数据无法正确和VertexArray挂钩
* 后续使用glVertexAttribPointer, 注意一定需要和正确的VBO, VAO挂钩
* VAO设置为0可以进行解绑

### glVertexAttribPointer
```C++
void glVertexAttribPointer(ELEMENT_INDEX, ELEMENT_COUNT, SHADER_DATA_BASE_TYPE, IS_NORMALIZED, ELEMENT_ALL_OFFSET, ELEMENT_CURRENT_OFFSET);
// ELEMENT_INDEX:          当前属性在当前一个顶点里的index, 从0开始
// ELEMENT_COUNT:          属性中的数据个数
// SHADER_DATA_BASE_TYPE:  属性中的数据的类型(推荐为GL_FLOAT, 如果传入整数会将其转换为浮点值)
// IS_NORMALIZED:          是否对属性的数据进行标准化(归一化: [0, 1]或者[-1, 1])
// ELEMENT_ALL_OFFSET:     两个相邻顶点之间间隔多少字节(整个顶点的字节大小)(比如顶点只有一个属性, 且为3个float, 那么此时就是3 * 4 byte)
// ELEMENT_CURRENT_OFFSET: 当前属性数据在顶点中的起点位移位置(比如元素1为3个float, 那么此时就是0, 下一个元素就是 3 * 4)
```

* 设置每一个顶点数据的各个元素布局(base为float类型)
* 适用类型: Float(2/3/4), Mat3/4 -> GL_FLOAT

### glVertexAttribIPointer
```C++
void glVertexAttribIPointer(ELEMENT_INDEX, ELEMENT_COUNT, SHADER_DATA_BASE_TYPE, ELEMENT_ALL_OFFSET, ELEMENT_CURRENT_OFFSET);
// SHADER_DATA_BASE_TYPE: 元素中的数据的类型只能为整数类型(GL_INT, GL_BOOL)
```

* 设置每一个顶点数据的各个元素布局(base为int类型)
* 适用类型: Bool, Int(2/3/4) -> GL_BOOL, GL_INT

### glEnableVertexAttribArray
```C++
void glEnableVertexAttribArray(ELEMENT_INDEX);
```

* 是否启用对应VAO顶点布局中的对应元素

### glUseProgram
```C++
void glUseProgram(PROGRAM_OBJ);
```

* 在绘制流程中使用shader程序, 方便后续的绘制操作


### glDrawArrays
```C++
void glDrawArrays(DRAW_BASE_TYPE, START_VERTEX_INDEX, DRAW_VERTEX_COUNT);
// DRAW_BASE_TYPE 绘制的图元类型
//   GL_TRIANGLES 三角形(逆时针)
// START_VERTEX_INDEX: 绘制的起始顶点
// DRAW_VERTEX_COUNT:  绘制多少个顶点
```

* 按照按顶点数组顺序直接画
* 使用前注意bindVAO对象

### glDrawElements
```C++
void glDrawElements(DRAW_BASE_TYPE, DRAW_INDEX_COUNT, INDEX_DATA_TYPE, BYTE_OFFSET);
// DRAW_INDEX_COUNT: 要使用索引缓冲区的索引数量
// INDEX_DATA_TYPE:  索引缓冲区中数据的类型
//   GL_UNSIGNED_INT -> uint32_t
// BYTE_OFFSET:      索引缓冲区的字节偏移, 通常为nullptr
```

* 按照索引数组指定的顺序绘制
* 使用前注意bindVAO对象

### glGetUniformLocation
```C++
Glint glGetUniformLocation(PROGRAM_OBJ, "uniform name");
```

* 返回shader程序中全局共享的uniform变量地址

### glUniformXXX
* CPU端给uniform变量上传对应类型的值
* 需要在glUseProgram后进行使用上传

```C++
glUniform4f(UNIFORM_LOCATION, r, g, b, a);
```

### Context共享资源
* glfw中可通过创建窗口时传入想要共享openglContext窗口的句柄达成共享, 切换OpenGLContext使用`glfwMakeContextCurrent`.
    - 其中, glfw中允许共享的对象存在: shaderProgram, VBO, EBO, Texture.
    - 不允许共享的对象: VAO, FBO...