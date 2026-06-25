#include <gtest/gtest.h>
#include <iostream>
#include <GLFW/glfw3.h>

class TestWindowRender : public testing::Test
{
};

TEST_F(TestWindowRender, glfw)
{
    std::cout << "这里是测试glfw的测试代码" << std::endl;

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
        // glfw利用下面的函数进行处理**窗口事件**
        glfwPollEvents();
    }

    glfwTerminate();  // 终止glfw, 清理资源
}