#pragma once

#include <utility>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GLFWwindow;

struct Vector2
{
    float x, y;

    Vector2() : x{0.0f}, y{0.0f}
    {
    }
    Vector2(float _x, float _y) : x{_x}, y{_y}
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

void setWindowResizeEnable(GLFWwindow* const _window, bool _enable);

void destroyWindow(GLFWwindow* _window);

bool renderContextInit(GLFWwindow* _window);

bool windowShouldClose(GLFWwindow* const _window);

void swapBuffers(GLFWwindow* const _window);

void pollEvents();

}  // namespace glfw_opengl

GLuint createShaderProgram(std::string const& _vertexShaderSrc,
                           std::string const& _fragmentShaderSrc);