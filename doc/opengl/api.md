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
//SHADER_TYPE:
// GL_VERTEX_SHADER 顶点着色器
```
* 创建着色器

### glShaderSource
```C++
glShaderSource(SHADER_OBJ, SOURCE_COUNT, SOURCES, SOURCE_LENS);
// SHADER_OBJ: 通过glCreateShader创建出的shader对象
// SOURCE_COUNT 源码存在几段, 一般为一段(opengl允许你拆成多段，然后将其合并为一个源码处理)
// SOURCES const char*[] 源码字符串
// SOURCE_LENS int* 源码段数的各自长度, 可以设置为nullptr(要求:每个源码字符串都是普通 C 字符串，以 \0 结尾)
```

* 设置着色器源码