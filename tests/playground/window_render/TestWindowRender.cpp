#include <iostream>
#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class TestWindowRender : public testing::Test
{
};

void glfwOpenGlInit()
{
    // 可在初始化前注册glfw错误回调函数, 函数类型为void(int error, const char* description)
    // glfwSetErrorCallback

    // glfwInit glfw初始化
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "glfw初始化失败!" << std::endl;
        ASSERT_TRUE(false);
    }
    else
    {
        std::cout << "glfw初始化成功!" << std::endl;
    }

    // glfw设置默认渲染opengl上下文的版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // 大版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // 小版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // profile
    // 使用core的理由: 只保留现代 OpenGL 推荐使用的功能，把很多旧式 OpenGL 功能去掉
}

void setWindowCenterPos(GLFWwindow* const _window)
{
    // 获取主显示器
    GLFWmonitor* const monitor{glfwGetPrimaryMonitor()};
    // 获取主显示器相关模式
    GLFWvidmode const* monitorMode{glfwGetVideoMode(monitor)};
    int const screenWidth{monitorMode->width};
    int const screenHeight{monitorMode->height};

    int windowWidth{};
    int windowHeight{};
    glfwGetWindowSize(_window, &windowWidth, &windowHeight);

    // 设置窗口位置
    // 设置到屏幕中间的位置
    glfwSetWindowPos(
        _window, screenWidth / 2 - windowWidth / 2, screenHeight / 2 - windowHeight / 2);
}

GLFWwindow* createGlfwWindow()
{
    // 创建glfw窗口
    // 创建一个16:9的小窗口
    int const windowWidth{1280};
    int const windowHeight{720};

    // 注意此处传入monitor是指在哪个显示器上创建全屏窗口
    GLFWwindow* const glfwWindow{
        glfwCreateWindow(windowWidth, windowHeight, "TestWindowRender", nullptr, nullptr)};
    if (glfwWindow == nullptr)
    {
        std::cerr << "glfw创建窗口失败!" << std::endl;
        glfwTerminate();  // 终止glfw, 清理资源
    }
    else
    {
        std::cout << "glfw窗口创建成功!" << std::endl;
        setWindowCenterPos(glfwWindow);
    }

    return glfwWindow;
}

void glfwOpenGLInit(GLFWwindow* _window)
{
    // 创建完窗口后, glfw设置指定窗口为当前线程的opengl上下文
    // 多窗口的切换渲染也利用此进行实现, 但是gl的全局函数加载只需要一次即可
    glfwMakeContextCurrent(_window);

    // 设置完毕后通过获取opengl上下文函数地址, 初始化glad, 使glad加载现代opengl函数
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        std::cerr << "glad 加载OpenGL函数失败!" << std::endl;
        glfwTerminate();
        ASSERT_TRUE(false);
    }

    // 打印加载的opengl上下文信息
    std::cout << "OpenGL的实现方[vendor]:    " << glGetString(GL_VENDOR)
              << std::endl;  // opengl的实现方
    std::cout << "实际渲染的GPU[renderer]:   " << glGetString(GL_RENDERER)
              << std::endl;  // 用来实际渲染的GPU
    std::cout << "OpenGL的实际版本[version]: " << glGetString(GL_VERSION)
              << std::endl;  // opengl的实际版本
}

TEST_F(TestWindowRender, windowRender_glfw_opengl)
{
    std::cout << "这里是测试windowRender_glfw_opengl的测试代码" << std::endl;

    glfwOpenGlInit();

    GLFWwindow* const glfwWindow{createGlfwWindow()};
    ASSERT_TRUE(glfwWindow != nullptr);

    glfwOpenGLInit(glfwWindow);

    GLuint vertexArray{};
    GLuint shaderProgram{};
    GLint uColorLocation{};

    // opengl 渲染数据准备
    {
        // 开始渲染管线中需要准备的shader相关数据
        // 1. 创建vertexShader
        GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
        std::string vertexShaderSrc{R"(
        #version 330 core
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 color;

        out vec3 vColor;

        void main() {
            vColor = color;
            gl_Position = vec4(position, 1);
        }
    )"};
        char const* cVertexShaderSrc{vertexShaderSrc.c_str()};
        // 给vertex shader设置源码
        glShaderSource(vertexShader, 1, &cVertexShaderSrc, nullptr);

        // 2. 创建fragmentShader
        GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
        std::string fragmentShaderSrc{R"(
        #version 330 core
        out vec4 color;

        in vec3 vColor;

        uniform vec4 uColor;

        void main() {
            color = vec4(vColor, 1.0) * uColor;
        }
    )"};
        char const* cfragmentShaderSrc{fragmentShaderSrc.c_str()};
        glShaderSource(fragmentShader, 1, &cfragmentShaderSrc, nullptr);

        // 3. 编译两个shader
        GLuint shaders[2]{vertexShader, fragmentShader};
        for (int i{0}; i < 2; ++i)
        {
            // 编译对应shader
            glCompileShader(shaders[i]);

            // 检查编译shader是否出错:
            GLint isCompileSuccess{};
            glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &isCompileSuccess);
            if (!isCompileSuccess)
            {
                // 打印出错日志
                GLchar buffer[512]{};
                glGetShaderInfoLog(shaders[i], 512, nullptr, buffer);
                std::cerr << "第" << i + 1 << "个shader编译失败:\n" << buffer << std::endl;

                glfwTerminate();
                ASSERT_TRUE(isCompileSuccess);
            }
            else
            {
                std::cout << "第" << i + 1 << "个shader编译成功!" << std::endl;
            }
        }

        // 4. 创建shader program并进行添加
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        // 5. shader program进行链接
        glLinkProgram(shaderProgram);
        // 检查链接是否出现问题
        GLint isLinkSuccess{};
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinkSuccess);
        if (!isLinkSuccess)
        {
            GLchar buffer[512]{};
            glGetProgramInfoLog(shaderProgram, 512, nullptr, buffer);
            std::cerr << "shader程序链接失败:\n" << buffer << std::endl;

            glfwTerminate();
            ASSERT_TRUE(isLinkSuccess);
        }
        else
        {
            std::cout << "shader程序链接成功!" << std::endl;
        }

        // 6. 链接成功后删除冗余的shader obj
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // 准备vertex data, 从CPU端上传到GPU
        // vbo + vao
        GLuint vertexBuffer{};
        glGenBuffers(1, &vertexBuffer);
        // glCreateBuffers(1, &vertexBuffer);  // 330core 不允许使用

        // bind vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        // 按照顶点数据顺序, 逆时针旋转
        float vertices[]{0.0f,
                         0.5f,
                         0.0f,
                         1.0,
                         0.0,
                         0.0,
                         -0.5f,
                         -0.5f,
                         0.0f,
                         0.0,
                         1.0,
                         0.0,
                         0.5f,
                         -0.5f,
                         0.0f,
                         0.0,
                         0.0,
                         1.0};
        // 静态一次性上传
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, vertices, GL_STATIC_DRAW);
        // 解绑vertex buffer的绑定
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // 创建ebo, 索引缓冲区, 指定顶点的绘制顺序
        GLuint indexBuffer{};
        glGenBuffers(1, &indexBuffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        uint32_t indices[]{0, 1, 2};

        // 静态上传索引缓冲区数据
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 9, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // 创建顶点数组对象, 方便后续的上传顶点布局和绘制数组对象
        glGenVertexArrays(1, &vertexArray);
        // glCreateVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        // 开始创建顶点布局
        // 第0个属性, 属性中存在三个基础值, 基础值类型, 是否标准化, 两个顶点之间的字节间隔,
        // 属性的起始位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
        // 启用0属性
        glEnableVertexAttribArray(0);
        // color属性, 起始位置在顶点数据之后
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // 获取uniform变量在shader程序中的地址
        uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    }

    // 窗口循环
    // 利用窗口是否关闭来进行循环
    while (!glfwWindowShouldClose(glfwWindow))
    {
        // 设置清屏颜色
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // 清理当前启用的帧缓冲区, 默认为窗口后缓冲区
        glClear(GL_COLOR_BUFFER_BIT);

        // renderer
        {
            // 使用gl shader程序, 便于渲染
            glUseProgram(shaderProgram);
            // 上传uniform 变量的值
            glUniform4f(uColorLocation, 1.0, 1.0, 1.0, 1.0);
            // gl 绘制顶点数组, 按照数组对象中的顶点顺序依次绘制
            glBindVertexArray(vertexArray);
            // glDrawArrays(GL_TRIANGLES, 0, 3);  // 从0顶点开始, 绘制3个顶点
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);  // 通过索引缓冲区绘制
        }

        // 交换窗口的颜色缓冲区(双缓冲区, 防止屏幕出现残影等)
        glfwSwapBuffers(glfwWindow);

        // glfw利用下面的函数进行处理**窗口事件**
        glfwPollEvents();
    }

    glfwTerminate();  // 终止glfw, 清理资源
}