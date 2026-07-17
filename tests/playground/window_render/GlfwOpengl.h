#pragma once

#include <utility>
#include <string_view>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GLFWwindow;

struct Vec2
{
    float x, y;
    Vec2() : x{0.0f}, y{0.0f}
    {
    }
    Vec2(float _x, float _y) : x{_x}, y{_y}
    {
    }
};

struct Veci2
{
    int x, y;
    Veci2() : x{0}, y{0}
    {
    }
    Veci2(int _x, int _y) : x{_x}, y{_y}
    {
    }

    bool operator==(Veci2 const& _other) const
    {
        return this->x == _other.x && this->y == _other.y;
    }
};

struct Vec4
{
    float r, g, b, a;
    Vec4() : r{0.0f}, g{0.0f}, b{0.0f}, a{0.0f}
    {
    }
    Vec4(float _r, float _g, float _b, float _a) : r{_r}, g{_g}, b{_b}, a{_a}
    {
    }
};

namespace glfw_opengl
{

bool windowRenderInit();

void windowShutdown();

std::pair<int, int> getPrimaryMonitorSize();

void setWindowCenterPos(GLFWwindow* const _window);

GLFWwindow* createWindow(std::string_view _title,
                         GLFWwindow* _shareWindow,
                         int _width = 1280,
                         int _height = 720);

GLFWwindow* createWindow(std::string_view _title, int _width = 1280, int _height = 720);

GLFWwindow* createWindow(std::string_view _title, float _widthRatio, float _heightRatio);

std::pair<int, int> getWindowSize(GLFWwindow* const _window);

void setWindowResizeEnable(GLFWwindow* const _window, bool _enable);

void destroyWindow(GLFWwindow* _window);

bool renderContextInit(GLFWwindow* _window, bool const depthTest = false);

bool windowShouldClose(GLFWwindow* const _window);

void swapBuffers(GLFWwindow* const _window);

void pollEvents();

}  // namespace glfw_opengl

GLuint createShaderProgram(std::string const& _vertexShaderSrc,
                           std::string const& _fragmentShaderSrc);