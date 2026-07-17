#include <iostream>
#include <gtest/gtest.h>

#include "playground/window_render/GlfwOpengl.h"

class TestWindowRender : public testing::Test
{
};

TEST_F(TestWindowRender, windowRender_glfw_opengl_helloworld)
{
    std::cout << "<< 这里是测试windowRender_glfw_opengl的helloworld >>" << std::endl;

    glfw_opengl::windowRenderInit();

    GLFWwindow* const glfwWindow{glfw_opengl::createWindow("TestWindowRender")};
    if (glfwWindow == nullptr)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }

    glfw_opengl::renderContextInit(glfwWindow);

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
    while (!glfw_opengl::windowShouldClose(glfwWindow))
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

        glfw_opengl::swapBuffers(glfwWindow);
        glfw_opengl::pollEvents();
    }

    glfw_opengl::windowShutdown();
}

TEST_F(TestWindowRender, windowRender_glfw_opengl_windows)
{
    std::cout << "<< 这里是测试windowRender_glfw_opengl的多窗口绘制 >>" << std::endl;

    std::vector<std::pair<GLFWwindow*, std::string>> windows{};

    bool const isInitSuccess{glfw_opengl::windowRenderInit()};
    ASSERT_TRUE(isInitSuccess);

    GLFWwindow* const mainWindow{glfw_opengl::createWindow("主窗口")};
    if (mainWindow == nullptr)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }

    bool const isRenderContextInitSuccess{glfw_opengl::renderContextInit(mainWindow)};
    if (!isRenderContextInitSuccess)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }

    windows.push_back(std::make_pair(mainWindow, "main"));
    GLFWwindow* otherWindow{glfw_opengl::createWindow("次窗口", mainWindow)};
    windows.push_back(std::make_pair(otherWindow, "other"));

    GLuint triangleVAO{};
    GLuint squareVAO{};
    GLuint shaderProgram{};

    // render data
    {
        // shader source
        std::string const vertexShaderSource{R"(
            #version 330 core
            layout(location=0) in vec3 position;
            layout(location=1) in vec4 color;

            out vec4 vColor;

            void main() {
                vColor = color;
                gl_Position = vec4(position, 1.0f);
            }
        )"};
        std::string const fragmentShaderSource{R"(
            #version 330 core
            in vec4 vColor;

            out vec4 color;

            void main() {
                color = vColor;
            }
        )"};
        shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
        if (shaderProgram == 0)
        {
            glfw_opengl::windowShutdown();
            ASSERT_TRUE(false);
        }

        {
            GLuint triangleVBO{};
            GLuint squareVBO{};

            GLuint triangleEBO{};
            GLuint squareEBO{};

            // vertex data
            std::vector<float> triangleVertice{
                0.0f,
                0.5f,
                0.0f,
                1.0f,
                0.0,
                0.0,  // vertex1
                -0.5f,
                -0.5f,
                0.0f,
                0.0f,
                1.0f,
                0.0f,  // vertex2
                0.5f,
                -0.5f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,  // vertex3
            };
            std::vector<float> squareVertice{
                -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  // vertex1
                -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // vertex2
                0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // vertex3
                0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  // vertex4
            };

            // vbo + ebo + vao
            glfwMakeContextCurrent(mainWindow);
            glGenBuffers(1, &triangleVBO);
            glGenVertexArrays(1, &triangleVAO);
            glBindVertexArray(triangleVAO);

            glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
            glBufferData(GL_ARRAY_BUFFER,
                         triangleVertice.size() * sizeof(float),
                         triangleVertice.data(),
                         GL_STATIC_DRAW);

            // layout
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(1);

            // index
            std::vector<uint32_t> triangleIndices{0, 1, 2};
            glGenBuffers(1, &triangleEBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(uint32_t) * triangleIndices.size(),
                         triangleIndices.data(),
                         GL_STATIC_DRAW);

            glfwMakeContextCurrent(otherWindow);
            glGenBuffers(1, &squareVBO);
            glGenVertexArrays(1, &squareVAO);
            glBindVertexArray(squareVAO);

            glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
            glBufferData(GL_ARRAY_BUFFER,
                         squareVertice.size() * sizeof(float),
                         squareVertice.data(),
                         GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(1);

            std::vector<uint32_t> squareIndices{0, 1, 2, 2, 3, 0};
            glGenBuffers(1, &squareEBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(uint32_t) * squareIndices.size(),
                         squareIndices.data(),
                         GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    }

    while (!glfw_opengl::windowShouldClose(mainWindow))
    {
        glfw_opengl::pollEvents();

        // renderer main
        {
            glfwMakeContextCurrent(mainWindow);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);

            // render 三角形
            glUseProgram(shaderProgram);
            glBindVertexArray(triangleVAO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        }

        glfw_opengl::swapBuffers(mainWindow);

        // render other
        auto it{windows.begin()};
        ++it;
        for (; it != windows.end();)
        {
            GLFWwindow* const curWindow{it->first};
            if (glfw_opengl::windowShouldClose(curWindow))
            {
                // 如果关闭了当前窗口
                glfw_opengl::destroyWindow(curWindow);
                it = windows.erase(it);
                continue;
            }

            // renderer
            {
                glfwMakeContextCurrent(curWindow);
                glClearColor(1.0, 1.0, 1.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);

                // render 四边形
                glUseProgram(shaderProgram);
                glBindVertexArray(squareVAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }

            glfw_opengl::swapBuffers(curWindow);

            ++it;
        }
    }

    // window clear
    for (auto& [window, _] : windows)
    {
        glfw_opengl::destroyWindow(window);
    }

    glfw_opengl::windowShutdown();
}

TEST_F(TestWindowRender, windowRender_glfw_opengl_backWhiteCheckerBoard)
{
    std::cout << "<< 这里是测试windowRender_glfw_opengl渲染白黑棋盘背景 >>" << std::endl;

    ASSERT_TRUE(glfw_opengl::windowRenderInit());
    auto const size{glfw_opengl::getPrimaryMonitorSize()};
    GLFWwindow* const mainWindow{
        glfw_opengl::createWindow("backWhiteCheckerBoard", size.first / 4, size.first / 4)};
    if (mainWindow == nullptr)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }
    bool const isInitSuccess{glfw_opengl::renderContextInit(mainWindow)};
    if (!isInitSuccess)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }

    // 3 * 3 的黑白棋盘
    // 渲染思路: 准备一个四边形纯色渲染即可, 利用uniform变量控制此四边形渲染的位置和颜色即可
    // shader source
    std::string const vertexShaderSource{R"(
        #version 330 core
        layout(location=0) in vec3 position;

        uniform float uScale;
        uniform vec2 uTranslation;

        void main() {
            gl_Position = vec4(position.x * uScale + uTranslation.x, position.y * uScale + uTranslation.y, position.z * uScale, 1.0);
        }
    )"};
    std::string const fragmentShaderSource{R"(
        #version 330 core
        uniform vec4 uColor;

        out vec4 color;

        void main() {
            color = uColor;
        }
    )"};
    GLuint shaderProgram{createShaderProgram(vertexShaderSource, fragmentShaderSource)};
    if (shaderProgram == 0)
    {
        glfw_opengl::windowShutdown();
        ASSERT_TRUE(false);
    }

    GLint uScaleLoc{glGetUniformLocation(shaderProgram, "uScale")};
    GLint uTranslationLoc{glGetUniformLocation(shaderProgram, "uTranslation")};
    GLint uColorLoc{glGetUniformLocation(shaderProgram, "uColor")};

    GLuint squareVAO{};
    {
        std::vector<float> squareVertice{
            0.0f,
            0.0f,
            0.0f,  // vertex1
            0.0f,
            -2.0f,
            0.0f,  // vertex2
            2.0f,
            -2.0f,
            0.0f,  // vertex3
            2.0f,
            0.0f,
            0.0f,  // vertex4
        };
        GLuint squareVBO{};
        GLuint squareEBO{};

        glGenBuffers(1, &squareVBO);
        glGenVertexArrays(1, &squareVAO);
        glBindVertexArray(squareVAO);

        glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     squareVertice.size() * sizeof(float),
                     squareVertice.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);

        std::vector<uint32_t> squareIndices{0, 1, 2, 2, 3, 0};
        glGenBuffers(1, &squareEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(uint32_t) * squareIndices.size(),
                     squareIndices.data(),
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    float const step{1.0f / 3.0f};
    Vec2 playerTranslation{-1.0f + 2 * step + step * 0.5f, 1.0f - 2 * step - step * 0.5f};
    glfwSetWindowUserPointer(mainWindow, &playerTranslation);
    // 设置窗口事件的回调函数
    // GLFWkeyfun: void (*)(GLFWwindow *, int, int, int, int) (aka void (*)(GLFWwindow *, int, int,
    // int, int))
    glfwSetKeyCallback(mainWindow,
                       [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
                           float const speed{0.1f};
                           Vec2& playerTranslation{*(Vec2*)glfwGetWindowUserPointer(_window)};

                           if (_action == GLFW_PRESS)
                           {
                               if (_key == GLFW_KEY_RIGHT)
                               {
                                   playerTranslation.x += speed;
                               }

                               if (_key == GLFW_KEY_LEFT)
                               {
                                   playerTranslation.x -= speed;
                               }

                               if (_key == GLFW_KEY_DOWN)
                               {
                                   playerTranslation.y -= speed;
                               }

                               if (_key == GLFW_KEY_UP)
                               {
                                   playerTranslation.y += speed;
                               }
                           }
                       });

    while (!glfw_opengl::windowShouldClose(mainWindow))
    {
        glfw_opengl::pollEvents();

        // renderer
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // render
        glUseProgram(shaderProgram);
        glBindVertexArray(squareVAO);
        glUniform1f(uScaleLoc, step);

        // glUniform2f(uTranslationLoc, -1.0f, 1.0f);
        // glUniform4f(uColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // glUniform2f(uTranslationLoc, -1.0f + 2 * step, 1.0f - 2 * step);
        // glUniform4f(uColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        for (int x{0}; x < 3; ++x)
        {
            for (int y{0}; y < 3; ++y)
            {
                glUniform2f(uTranslationLoc, -1.0f + x * step * 2, 1.0f - y * step * 2);
                // * 2的原因是x/y的范围为[-1, 1], 长度为2

                if ((x + y * 3) % 2 == 0)
                {
                    // 偶数个, 白色
                    glUniform4f(uColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    // 奇数个, 黑色
                    glUniform4f(uColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
                }

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }

        // player
        glUniform1f(uScaleLoc, step * 0.5f);
        glUniform2f(uTranslationLoc, playerTranslation.x, playerTranslation.y);
        glUniform4f(uColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfw_opengl::swapBuffers(mainWindow);
    }

    glfw_opengl::windowShutdown();
}

TEST_F(TestWindowRender, windowRender_glfw_opengl_font)
{
    std::cout << "<< 这里是测试windowRender_glfw_opengl渲染字体 >>" << std::endl;
    glfw_opengl::windowRenderInit();
    GLFWwindow* const window{glfw_opengl::createWindow("渲染字体", 360, 640)};
    glfw_opengl::setWindowResizeEnable(window, false);
    glfw_opengl::renderContextInit(window);

    // render data
    GLuint shaderProgram{};
    GLuint vao{};
    {
        // create shaderProgram
        std::string vertexShaderSource{R"(
            #version 330 core
            layout(location = 0) in vec2 position;

            uniform vec2 uScale;
            uniform vec2 uTranslation;

            void main(){
                gl_Position = vec4(position.x * uScale.x + uTranslation.x, position.y * uScale.y + uTranslation.y, 0.0, 1.0);
            }
        )"};
        std::string fragmentShaderSource{R"(
            #version 330 core

            uniform vec4 uColor;

            out vec4 color;

            void main() {
                color = uColor;
            }
        )"};

        shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

        // vao + vbo + ebo
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //  0-----2
        //  |  /  |
        //  1-----3
        std::vector<float> vertices{
            -0.5,
            0.5,  // vertex1
            -0.5,
            -0.5,  // vertx2
            0.5,
            0.5,  // vertex3
            0.5,
            -0.5,  // vertex4
        };

        glBufferData(
            GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        GLuint ebo{};
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        std::vector<uint32_t> indices{0, 1, 2, 2, 1, 3};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(uint32_t) * indices.size(),
                     indices.data(),
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    GLint uScale{glGetUniformLocation(shaderProgram, "uScale")};
    GLint uTranslation{glGetUniformLocation(shaderProgram, "uTranslation")};
    GLint uColor{glGetUniformLocation(shaderProgram, "uColor")};

    float fontSize{0.1f};

    /*
    A:        B:        C:        D:        E:        F:        G:        H:        I
    0,1,1,1,0 1,1,1,1,0 0,1,1,1,1 1,1,1,1,0 1,1,1,1,1 1,1,1,1,1 0,1,1,1,1 1,0,0,0,1 1,1,1,1,1
    1,0,0,0,1 1,0,0,0,1 1,0,0,0,0 1,0,0,0,1 1,0,0,0,0 1,0,0,0,0 1,0,0,0,0 1,0,0,0,1 0,0,1,0,0
    1,1,1,1,1 1,1,1,1,0 1,0,0,0,0 1,0,0,0,1 1,1,1,1,0 1,1,1,1,0 1,0,1,1,1 1,1,1,1,1 0,0,1,0,0
    1,0,0,0,1 1,0,0,0,1 1,0,0,0,0 1,0,0,0,1 1,0,0,0,0 1,0,0,0,0 1,0,0,0,1 1,0,0,0,1 0,0,1,0,0
    1,0,0,0,1 1,1,1,1,0 0,1,1,1,1 1,1,1,1,0 1,1,1,1,1 1,0,0,0,0 0,1,1,1,0 1,0,0,0,1 1,1,1,1,1
    J:        k:        L:        M:        N:        O:        P:        Q:        R:
    0,0,1,1,1 1,0,0,0,1 1,0,0,0,0 1,0,0,0,1 1,0,0,0,1 0,1,1,1,0 1,1,1,1,0 0,1,1,1,0 1,1,1,1,0
    0,0,0,1,0 1,0,0,1,0 1,0,0,0,0 1,1,0,1,1 1,1,0,0,1 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1
    0,0,0,1,0 1,1,1,0,0 1,0,0,0,0 1,0,1,0,1 1,0,1,0,1 1,0,0,0,1 1,1,1,0,0 1,0,1,0,1 1,1,1,1,0
    1,0,0,1,0 1,0,0,1,0 1,0,0,0,0 1,0,0,0,1 1,0,0,1,1 1,0,0,0,1 1,0,0,0,0 1,0,0,1,0 1,0,0,1,0
    0,1,1,0,0 1,0,0,0,1 1,1,1,1,1 1,0,0,0,1 1,0,0,0,1 0,1,1,1,0 1,0,0,0,0 0,1,1,0,1 1,0,0,0,1
    S:        T:        U:        V         W:        X:        Y:        Z:
    0,1,1,1,1 1,1,1,1,1 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1 1,1,1,1,1
    1,0,0,0,0 0,0,1,0,0 1,0,0,0,1 1,0,0,0,1 1,0,0,0,1 0,1,0,1,0 0,1,0,1,0 0,0,0,1,0
    0,1,1,1,0 0,0,1,0,0 1,0,0,0,1 1,0,0,0,1 1,0,1,0,1 0,0,1,0,0 0,0,1,0,0 0,0,1,0,0
    0,0,0,0,1 0,0,1,0,0 1,0,0,0,1 0,1,0,1,0 1,1,0,1,1 0,1,0,1,0 0,0,1,0,0 0,1,0,0,0
    1,1,1,1,0 0,0,1,0,0 0,1,1,1,0 0,0,1,0,0 1,0,0,0,1 1,0,0,0,1 0,0,1,0,0 1,0,0,0,0
    .:        ,:        ::        !:        ?:        -:
    0,0,0,0,0 0,0,0,0,0 0,0,0,0,0 0,0,1,0,0 0,1,1,1,0 0,0,0,0,0
    0,0,0,0,0 0,0,0,0,0 0,0,1,0,0 0,0,1,0,0 1,0,0,0,1 0,0,0,0,0
    0,0,0,0,0 0,0,0,0,0 0,0,0,0,0 0,0,1,0,0 0,0,0,1,0 1,1,1,1,1
    0,0,0,0,0 0,0,1,0,0 0,0,1,0,0 0,0,0,0,0 0,0,0,0,0 0,0,0,0,0
    0,0,1,0,0 0,1,0,0,0 0,0,0,0,0 0,0,1,0,0 0,0,1,0,0 0,0,0,0,0
    */
    std::unordered_map<char, std::vector<uint8_t>> fontMap{
        {'A', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}},
        {'B', {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
        {'C', {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1}},
        {'D', {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
        {'E', {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}},
        {'F', {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
        {'G', {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
        {'H', {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}},
        {'I', {1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1}},
        {'J', {0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0}},
        {'K', {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1}},
        {'L', {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}},
        {'M', {1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}},
        {'N', {1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1}},
        {'O', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
        {'P', {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
        {'Q', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1}},
        {'R', {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1}},
        {'S', {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
        {'T', {1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}},
        {'U', {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
        {'V', {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0}},
        {'W', {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1}},
        {'X', {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1}},
        {'Y', {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}},
        {'Z', {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1}},
        {',', {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}},
        {' ', {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

    auto drawChar = [&](char _char, Vec2 const& _position, float _fontSize) {
        glUseProgram(shaderProgram);
        float const fontCharSize{_fontSize / 5.0f};
        glUniform2f(uScale, fontCharSize, fontCharSize);

        auto it{fontMap.find(_char)};
        if (it == fontMap.end())
        {
            it = fontMap.find(' ');
        }
        auto& renderFontCells{it->second};
        glBindVertexArray(vao);
        for (int x{0}; x < 5; ++x)
        {
            for (int y{0}; y < 5; ++y)
            {
                glUniform2f(uTranslation,
                            (x - 2) * fontCharSize + _position.x,
                            (2 - y) * fontCharSize + _position.y);
                if (renderFontCells[x + y * 5] == 1)
                {
                    glUniform4f(uColor, 1.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glUniform4f(uColor, 0.0f, 0.0f, 0.0f, 1.0f);
                }

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    };

    auto drawString{[&](std::string const& _str, Vec2 const& _position, float _fontSize) {
        float const fontCharSize{_fontSize / 5.0f};
        int strSize{static_cast<int>(_str.size())};
        int halfStrSize{strSize / 2};

        for (int i{0}; i < strSize; ++i)
        {
            float charTransfX{};
            if (i < halfStrSize)
            {
                charTransfX = (i - halfStrSize) * _fontSize + (i - halfStrSize + 1) * fontCharSize -
                    fontCharSize / 2.0f + _fontSize / 2.0f;
            }
            else
            {
                charTransfX = (i - halfStrSize + 1) * _fontSize + (i - halfStrSize) * fontCharSize +
                    fontCharSize / 2.0f - _fontSize / 2.0f;
            }

            drawChar(_str[i], {_position.x + charTransfX, _position.y}, _fontSize);
        }
    }};

    while (!glfw_opengl::windowShouldClose(window))
    {
        glfw_opengl::pollEvents();

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render font
        // drawChar('A', {}, fontSize);
        // drawChar('B', {fontSize + 0.01f, 0.0f}, fontSize);

        drawString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", {}, fontSize / 2.0f);
        drawString("     HELLO, WORLD    QIHAI", {0.0f, -fontSize - 0.03f}, fontSize / 3.0f);

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}