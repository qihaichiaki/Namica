#include "playground/window_render/GlfwOpengl.h"
#include "namica/math/Matrix.h"

#include <gtest/gtest.h>

class TestWindowRender : public testing::Test
{
};

TEST_F(TestWindowRender, cube_render)
{
    // 立方体渲染 - 3d渲染

    namica::Mat4 mat4{};

    // init
    glfw_opengl::windowRenderInit();
    GLFWwindow* const window{glfw_opengl::createWindow("cub render", 0.3f, 0.3f)};
    glfw_opengl::renderContextInit(window);

    while (!glfw_opengl::windowShouldClose(window))
    {
        glfw_opengl::pollEvents();

        // render
        glClearColor(0.961f, 0.961f, 0.863f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}