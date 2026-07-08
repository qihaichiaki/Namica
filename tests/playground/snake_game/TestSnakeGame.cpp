#include <iostream>
#include <unordered_map>
#include <chrono>
#include <list>
#include <random>
#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class TestSnakeGame : public testing::Test
{
};

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

bool windowRenderInit()
{
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "glfw初始化失败!" << std::endl;
        return false;
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

    return true;
}

void windowShutdown()
{
    glfwTerminate();  // 终止glfw, 清理资源
}

std::pair<int, int> getPrimaryMonitorSize()
{
    GLFWmonitor* const monitor{glfwGetPrimaryMonitor()};
    GLFWvidmode const* monitorMode{glfwGetVideoMode(monitor)};
    return {monitorMode->width, monitorMode->height};
}

void setWindowCenterPos(GLFWwindow* const _window)
{
    auto const size{getPrimaryMonitorSize()};
    int const screenWidth{size.first};
    int const screenHeight{size.second};

    int windowWidth{};
    int windowHeight{};
    glfwGetWindowSize(_window, &windowWidth, &windowHeight);
    glfwSetWindowPos(
        _window, screenWidth / 2 - windowWidth / 2, screenHeight / 2 - windowHeight / 2);
}

GLFWwindow* createWindow(std::string_view _title, int _width = 1280, int _height = 720)
{
    GLFWwindow* const glfwWindow{
        glfwCreateWindow(_width, _height, _title.data(), nullptr, nullptr)};
    if (glfwWindow == nullptr)
    {
        std::cerr << "glfw创建窗口失败!" << std::endl;
    }
    else
    {
        std::cout << "glfw窗口创建成功!" << std::endl;
        setWindowCenterPos(glfwWindow);
    }

    return glfwWindow;
}

GLFWwindow* createWindow(std::string_view _title, float _widthRatio, float _heightRatio)
{
    auto mainWindowSize{getPrimaryMonitorSize()};
    Veci2 windowSize{static_cast<int>(mainWindowSize.first * _widthRatio),
                     static_cast<int>(mainWindowSize.second * _heightRatio)};
    return createWindow(_title, windowSize.x, windowSize.y);
}

void setWindowResizeEnable(GLFWwindow* const _window, bool _enable)
{
    glfwSetWindowAttrib(_window, GLFW_RESIZABLE, _enable ? GLFW_TRUE : GLFW_FALSE);
}

void destroyWindow(GLFWwindow* _window)
{
    glfwDestroyWindow(_window);
}

bool renderContextInit(GLFWwindow* _window)
{
    glfwMakeContextCurrent(_window);
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        std::cerr << "glad 加载OpenGL函数失败!" << std::endl;
        return false;
    }

    std::cout << "OpenGL的实现方[vendor]:    " << glGetString(GL_VENDOR)
              << std::endl;  // opengl的实现方
    std::cout << "实际渲染的GPU[renderer]:   " << glGetString(GL_RENDERER)
              << std::endl;  // 用来实际渲染的GPU
    std::cout << "OpenGL的实际版本[version]: " << glGetString(GL_VERSION)
              << std::endl;  // opengl的实际版本

    // 启用纹理混合
    glEnable(GL_BLEND);
    // 经典混合函数, 实现透明效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool windowShouldClose(GLFWwindow* const _window)
{
    return glfwWindowShouldClose(_window);
}

void swapBuffers(GLFWwindow* const _window)
{
    glfwSwapBuffers(_window);
}

void pollEvents()
{
    glfwPollEvents();
}

}  // namespace glfw_opengl

GLuint createShaderProgram(std::string const& _vertexShaderSrc,
                           std::string const& _fragmentShaderSrc)
{
    GLuint shaderProgram{};

    GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};

    char const* vertexShaderSourceCStr{_vertexShaderSrc.c_str()};
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    char const* fragmentShaderSourceCStr{_fragmentShaderSrc.c_str()};
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);

    // Compile
    glCompileShader(vertexShader);
    GLint isCompileSuccess{};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompileSuccess);
    if (isCompileSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetShaderInfoLog(vertexShader, 512, nullptr, buffer);
        std::cerr << "shader编译失败: " << buffer << std::endl;

        return 0;
    }
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompileSuccess);
    if (isCompileSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetShaderInfoLog(fragmentShader, 512, nullptr, buffer);
        std::cerr << "shader编译失败: " << buffer << std::endl;

        return 0;
    }

    // shaderProgram
    shaderProgram = glCreateProgram();
    // attach
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link
    glLinkProgram(shaderProgram);
    GLint isLinkSuccess{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinkSuccess);
    if (isLinkSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetProgramInfoLog(shaderProgram, 512, nullptr, buffer);
        std::cerr << "shaderProgram链接失败: " << buffer << std::endl;

        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

namespace
{
// render data
GLuint shaderProgram{};
GLuint vao{};
GLint uScale{};
GLint uTranslation{};
GLint uColor{};

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
    {'0', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
    {'1', {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0}},
    {'2', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1}},
    {'3', {1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
    {'4', {1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0}},
    {'5', {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
    {'6', {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
    {'7', {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}},
    {'8', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}},
    {'9', {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0}},
    {' ', {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

// 全局状态变量
int const cellMaxWidth{16 * 2};
int const cellMaxHeight{18 * 2};
float const cellWidthScale{2.0f / cellMaxWidth};
float const cellHeightScale{2.0f / cellMaxHeight};

Vec4 uiColor{0.227f, 0.200f, 0.169f, 1.0f};
Vec4 wallColor{0.353f, 0.396f, 0.451f, 1.0f};
Vec4 snakeColor{0.0f, 0.0f, 1.0f, 1.0f};
Vec4 foodColor{1.0f, 0.0f, 0.0f, 1.0f};

enum class GameStatus
{
    start,
    run,
    over
};
GameStatus gameStatus{GameStatus::start};

// game start
float uiFontMax{0.045f};
float uiFontMin{0.035f};
float uiFontSpeed{0.0005f};
float uiFont{uiFontMin};
bool isFontEnlarge{true};

// game run
// 蛇的初始body
Veci2 snakeStartBody[3]{Veci2{8, 7}, Veci2{7, 7}, Veci2{6, 7}};
std::list<Veci2> snakeBody{};  //  使用链表数据结构
enum class SnakeDir
{
    right,
    left,
    up,
    down
};
SnakeDir snakeDir{SnakeDir::right};
float const snakeStartSpeed{0.3f};
float snakeSpeed{snakeStartSpeed};
bool haveFood{false};
Veci2 foodPos{};
int score{0};
bool eatFood{false};

// game over

// update func

// 游戏运行时状态清除
void gameRunStatusReset()
{
    snakeBody.clear();
    snakeBody.push_back(snakeStartBody[0]);
    snakeBody.push_back(snakeStartBody[1]);
    snakeBody.push_back(snakeStartBody[2]);

    snakeDir = SnakeDir::right;
    snakeSpeed = snakeStartSpeed;
    haveFood = false;
    score = 0;
    eatFood = false;
}

// 处理窗口的全局键盘事件
void onKeyEvent(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    // 游戏开始窗口
    if (gameStatus == GameStatus::start)
    {
        // 如果键盘按下
        if (_action == GLFW_PRESS)
        {
            gameStatus = GameStatus::run;
            gameRunStatusReset();
        }
    }

    if (gameStatus == GameStatus::run)
    {
        if (_action == GLFW_PRESS)
        {
            if (_key == GLFW_KEY_LEFT && snakeDir != SnakeDir::right)
            {
                snakeDir = SnakeDir::left;
            }
            if (_key == GLFW_KEY_RIGHT && snakeDir != SnakeDir::left)
            {
                snakeDir = SnakeDir::right;
            }
            if (_key == GLFW_KEY_UP && snakeDir != SnakeDir::down)
            {
                snakeDir = SnakeDir::up;
            }
            if (_key == GLFW_KEY_DOWN && snakeDir != SnakeDir::up)
            {
                snakeDir = SnakeDir::down;
            }
        }
    }

    if (gameStatus == GameStatus::over)
    {
        if (_action == GLFW_PRESS && _key == GLFW_KEY_R)
        {
            gameStatus = GameStatus::run;
            gameRunStatusReset();
        }
    }
}

// 更新字体缩放
void updateGameFontEnlarge(float _delta)
{
    static float uiTimeCount{0.0f};  // UI事件计时累计器

    uiTimeCount += _delta;
    if (uiTimeCount > 0.1f)
    {
        uiFont = isFontEnlarge ? uiFont + uiFontSpeed : uiFont - uiFontSpeed;
        if (uiFont > uiFontMax)
        {
            uiFont = uiFontMax;
            isFontEnlarge = false;
        }
        if (uiFont < uiFontMin)
        {
            uiFont = uiFontMin;
            isFontEnlarge = true;
        }
        uiTimeCount = 0.0f;
    }
}

Veci2 randomPos()
{
    static std::random_device rd;
    static std::mt19937 rng{rd()};
    std::uniform_int_distribution<int> distX(1, cellMaxWidth - 2);
    std::uniform_int_distribution<int> distY(1, cellMaxHeight - 2);
    return Veci2{distX(rng), distY(rng)};
}

// 更新游戏运行态
void updateGameRun(float _delta)
{
    static float timeCount{0.0f};
    timeCount += _delta;

    // check
    Veci2 head{snakeBody.front()};
    if (head.x <= 0 || head.x >= cellMaxWidth - 1 || head.y <= 0 || head.y >= cellMaxHeight - 1)
    {
        // 碰撞上墙壁了
        gameStatus = GameStatus::over;
        return;
    }
    // 检查是否撞在自己身上了
    auto it{snakeBody.begin()};
    ++it;
    while (it != snakeBody.end())
    {
        if (head == *(it))
        {
            // 撞到自己了
            gameStatus = GameStatus::over;
            return;
        }

        ++it;
    }

    // 是否吃到果子了
    if (haveFood)
    {
        if (head == foodPos)
        {
            // 吃到了
            ++score;
            eatFood = true;
            haveFood = false;
        }
    }
    else
    {
        foodPos = randomPos();
        haveFood = true;
    }

    // move
    if (timeCount > snakeSpeed)
    {
        switch (snakeDir)
        {
            case SnakeDir::right:
                head.x += 1;
                break;
            case SnakeDir::left:
                head.x -= 1;
                break;
            case SnakeDir::up:
                head.y -= 1;
                break;
            case SnakeDir::down:
                head.y += 1;
                break;
            default:
                break;
        }

        snakeBody.push_front(head);
        if (eatFood)
        {
            eatFood = false;
            snakeSpeed *= 0.85f;
        }
        else
        {
            snakeBody.pop_back();
        }

        timeCount = 0.0f;
    }
}

// render func
void drawChar(char _char,
              Vec2 const& _position,
              float _fontSize,
              Vec4 const& _color = Vec4{0.0f, 0.0f, 0.0f, 1.0f})
{
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
                glUniform4f(uColor, _color.r, _color.g, _color.b, _color.a);
            }
            else
            {
                glUniform4f(uColor, 0.0f, 0.0f, 0.0f, 0.0f);
            }

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
}

void drawString(std::string const& _str,
                Vec2 const& _position,
                float _fontSize,
                Vec4 const& _color = Vec4{0.0f, 0.0f, 0.0f, 1.0f})
{
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

        drawChar(_str[i], {_position.x + charTransfX, _position.y}, _fontSize, _color);
    }
}

void drawCell(Veci2 const& _position, Vec4 const& _color)
{
    glUseProgram(shaderProgram);
    glUniform2f(uScale, cellWidthScale, cellHeightScale);
    glUniform4f(uColor, _color.r, _color.g, _color.b, _color.a);

    static float cellHalfWidth{1.0f / cellMaxWidth};
    static float cellHalfHeight{1.0f / cellMaxHeight};
    static int cellMaxHalfWidth{cellMaxWidth / 2 - 1 < 0 ? 0 : cellMaxWidth / 2 - 1};
    static int cellMaxHalfHeith{cellMaxHeight / 2 - 1 < 0 ? 0 : cellMaxHeight / 2 - 1};

    Vec2 position{};

    if (_position.x <= cellMaxHalfWidth)
    {
        position.x = -2.0f * cellHalfWidth * (cellMaxHalfWidth - _position.x) - cellHalfWidth;
    }
    else
    {
        position.x = (_position.x - cellMaxHalfWidth - 1) * 2.0f * cellHalfWidth + cellHalfWidth;
    }

    if (_position.y <= cellMaxHalfHeith)
    {
        position.y = 2.0f * cellHalfHeight * (cellMaxHalfHeith - _position.y) + cellHalfHeight;
    }
    else
    {
        position.y = -2.0f * cellHalfHeight * (_position.y - cellMaxHalfHeith - 1) - cellHalfHeight;
    }

    glUniform2f(uTranslation, position.x, position.y);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// 绘制开始界面
void drawGameStart()
{
    drawString("SNAKE GAME", {0.0f, 0.3f}, 0.1f, uiColor);
    drawString("PRESS ANY KEY TO START", {0.0f, 0.0f}, uiFont, uiColor);
}

// 绘制结束界面
void drawGameOver()
{
    drawString("GAME OVER", {0.0f, 0.3f}, 0.1f, Vec4{1.0f, 0.0f, 0.0f, 1.0f});
    drawString("SCORE: " + std::to_string(score), {0.0f, 0.0f}, 0.08f, uiColor);
    drawString("PRESS R KEY TO REPLAY", {0.0f, -0.2f}, uiFont, uiColor);
}

// 绘制游戏运行界面
void drawGameRun()
{
    // 绘制墙壁
    for (int i{0}; i < cellMaxWidth; ++i)
    {
        drawCell(Veci2{i, 0}, wallColor);
        drawCell(Veci2{i, cellMaxHeight - 1}, wallColor);
    }
    for (int i{0}; i < cellMaxHeight; ++i)
    {
        drawCell(Veci2{0, i}, wallColor);
        drawCell(Veci2{cellMaxWidth - 1, i}, wallColor);
    }

    // 绘制蛇
    for (auto& snakePos : snakeBody)
    {
        drawCell(snakePos, snakeColor);
    }
    // 绘制食物
    if (haveFood)
    {
        drawCell(foodPos, foodColor);
    }
}

}  // namespace

TEST_F(TestSnakeGame, SnakeGameMain)
{
    glfw_opengl::windowRenderInit();
    GLFWwindow* const window{glfw_opengl::createWindow("SnakeGame", 1.0f / 4, 1.0f / 2)};
    glfw_opengl::setWindowResizeEnable(window, false);
    glfw_opengl::renderContextInit(window);

    // render准备
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
    uScale = glGetUniformLocation(shaderProgram, "uScale");
    uTranslation = glGetUniformLocation(shaderProgram, "uTranslation");
    uColor = glGetUniformLocation(shaderProgram, "uColor");

    // 设置键盘回调函数
    glfwSetKeyCallback(window, onKeyEvent);

    std::chrono::time_point<std::chrono::steady_clock> preTime{std::chrono::steady_clock::now()};

    while (!glfw_opengl::windowShouldClose(window))
    {
        // delta
        std::chrono::time_point<std::chrono::steady_clock> curTime{
            std::chrono::steady_clock::now()};
        float delta{std::chrono::duration<float>(curTime - preTime).count()};
        preTime = curTime;

        glfw_opengl::pollEvents();

        // update
        if (gameStatus == GameStatus::start)
        {
            updateGameFontEnlarge(delta);
        }
        if (gameStatus == GameStatus::run)
        {
            updateGameRun(delta);
        }
        if (gameStatus == GameStatus::over)
        {
            updateGameFontEnlarge(delta);
        }

        // render
        glClearColor(0.961f, 0.961f, 0.863f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (gameStatus == GameStatus::start)
        {
            drawGameStart();
        }
        if (gameStatus == GameStatus::run)
        {
            drawGameRun();
        }
        if (gameStatus == GameStatus::over)
        {
            drawGameRun();
            drawGameOver();
        }

        drawString("SCORE: " + std::to_string(score), {-0.85f, 0.98f}, 0.025f, uiColor);
        // 绘制游戏FPS
        drawString("FPS " + std::to_string(static_cast<int>(1.0f / delta)),
                   {0.85f, 0.98f},
                   0.025f,
                   uiColor);

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}