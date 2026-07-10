#include "playground/window_render/GlfwOpengl.h"

#include <gtest/gtest.h>

class TestWindowRender : public testing::Test
{
};

namespace
{
// math
class Mat4
{
public:
    Mat4() : m_data{}
    {
    }
    Mat4(float _v00,
         float _v01,
         float _v02,
         float _v03,
         float _v10,
         float _v11,
         float _v12,
         float _v13,
         float _v20,
         float _v21,
         float _v22,
         float _v23,
         float _v30,
         float _v31,
         float _v32,
         float _v33)
        : m_data{{_v00, _v01, _v02, _v03},
                 {_v10, _v11, _v12, _v13},
                 {_v20, _v21, _v22, _v23},
                 {_v30, _v31, _v32, _v33}}
    {
    }

    float& operator()(int _x, int _y)
    {
        return m_data[_x][_y];
    }

    Vec4 operator*(Vec4 const& _vec)
    {
        return Vec4{};
    }

    friend Vec4 operator*(Vec4 const& _vec, Mat4 const& _mat);

private:
    float m_data[4][4]{};
};

Vec4 operator*(Vec4 const& _vec, Mat4 const& _mat)
{
    return Vec4{};
}

}  // namespace

TEST_F(TestWindowRender, cube_render)
{
    // 立方体渲染 - 3d渲染

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