#include <iostream>
#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class TestWindowRender : public testing::Test
{
};

TEST_F(TestWindowRender, windowRender_glfw_opengl)
{
    std::cout << "这里是测试windowRender_glfw_opengl的测试代码" << std::endl;

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

    // 创建glfw窗口
    // 创建一个16:9的小窗口
    int const windowWidth{1280};
    int const windowHeight{720};
    GLFWwindow* const glfwWindow{
        glfwCreateWindow(windowWidth, windowHeight, "TestWindowRender", nullptr, nullptr)};
    if (glfwWindow == nullptr)
    {
        std::cerr << "glfw创建窗口失败!" << std::endl;
        glfwTerminate();  // 终止glfw, 清理资源
        ASSERT_TRUE(false);
    }
    else
    {
        std::cout << "glfw窗口创建成功!" << std::endl;
    }

    // 创建完窗口后, glfw设置指定窗口为当前线程的opengl上下文
    glfwMakeContextCurrent(glfwWindow);

    // 设置完毕后通过获取opengl上下文函数地址, 初始化glad, 使glad加载现代opengl函数
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        std::cerr << "glad 加载OpenGL函数失败!" << std::endl;
        glfwTerminate();
        ASSERT_TRUE(false);
    }

    // 打印加载的opengl上下文信息
    std::cout << "vendor: " << glGetString(GL_VENDOR) << std::endl;      // opengl的实现方
    std::cout << "renderer: " << glGetString(GL_RENDERER) << std::endl;  // 用来实际渲染的GPU
    std::cout << "version: " << glGetString(GL_VERSION) << std::endl;    // opengl的实际版本

    // 设置窗口位置
    int const displayScreenWidth{2560};
    int const displayScreenHeight{1440};
    // 设置到屏幕中间的位置
    glfwSetWindowPos(glfwWindow,
                     displayScreenWidth / 2 - windowWidth / 2,
                     displayScreenHeight / 2 - windowHeight / 2);

    // 窗口循环
    // 利用窗口是否关闭来进行循环
    while (!glfwWindowShouldClose(glfwWindow))
    {
        // 设置清屏颜色
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // 清理当前启用的帧缓冲区, 默认为窗口后缓冲区
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换窗口的颜色缓冲区(双缓冲区, 防止屏幕出现残影等)
        glfwSwapBuffers(glfwWindow);

        // glfw利用下面的函数进行处理**窗口事件**
        glfwPollEvents();
    }

    glfwTerminate();  // 终止glfw, 清理资源
}