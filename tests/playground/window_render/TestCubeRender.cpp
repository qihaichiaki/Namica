#include <unordered_map>
#include "playground/window_render/GlfwOpengl.h"
#include <namica/math/Matrix.h>
#include <namica/math/Utils.h>
#include <gtest/gtest.h>

class TestWindowRender : public testing::Test
{
};

namespace
{

// 材质: shader program + uniform
class Material
{
public:
    void setShaderProgram(GLuint _shaderProgram)
    {
        m_shaderProgram = _shaderProgram;
    }

    GLuint getShaderProgram() const
    {
        return m_shaderProgram;
    }

    void setParam(std::string const& _id, namica::Float const& _value)
    {
        m_floatData[_id] = _value;
    }

    void setParam(std::string const& _id, namica::Vec2 const& _value)
    {
        m_vec2Data[_id] = _value;
    }

    void setParam(std::string const& _id, namica::Vec3 const& _value)
    {
        m_vec3Data[_id] = _value;
    }

    void setParam(std::string const& _id, namica::Vec4 const& _value)
    {
        m_vec4Data[_id] = _value;
    }

    void bind()
    {
        glUseProgram(m_shaderProgram);
        // 遍历data, 依次设置值
        for (auto& [id, value] : m_floatData)
        {
            glUniform1f(getUniformLocation(id), value);
        }
        for (auto& [id, value] : m_vec2Data)
        {
            glUniform2f(getUniformLocation(id), value.x(), value.y());
        }
        for (auto& [id, value] : m_vec3Data)
        {
            glUniform3f(getUniformLocation(id), value.x(), value.y(), value.z());
        }
        for (auto& [id, value] : m_vec4Data)
        {
            glUniform4f(getUniformLocation(id), value.x(), value.y(), value.z(), value.w());
        }
    }

private:
    GLint getUniformLocation(std::string const& _id)
    {
        if (m_uniformlocation.find(_id) == m_uniformlocation.end())
        {
            m_uniformlocation[_id] = glGetUniformLocation(m_shaderProgram, _id.data());
        }

        return m_uniformlocation[_id];
    }

private:
    GLuint m_shaderProgram{};
    std::unordered_map<std::string, GLint> m_uniformlocation{};
    std::unordered_map<std::string, namica::Float> m_floatData{};
    std::unordered_map<std::string, namica::Vec2> m_vec2Data{};
    std::unordered_map<std::string, namica::Vec3> m_vec3Data{};
    std::unordered_map<std::string, namica::Vec4> m_vec4Data{};
};

// mesh, VBO + EBO + VAO

// 顶点元素
struct VertexElement
{
    GLenum dataType{GL_FLOAT};
    GLint dataSize{0};
    GLboolean normalized{GL_FALSE};
    GLsizei dataByte{};
    namica::UInt offset{};

    VertexElement(GLenum dataType, GLint _dataSize) : dataType{dataType}, dataSize{_dataSize}
    {
        switch (dataType)
        {
            case GL_FLOAT:
                dataByte = sizeof(float) * dataSize;
                break;
            case GL_INT:
                dataByte = sizeof(int) * dataSize;
                break;
            default:
                break;
        }
    }
};

// 顶点布局
class VertexLayout
{
public:
    VertexLayout(std::initializer_list<VertexElement> const& _elements)
        : m_elements{_elements}, m_stride{0}
    {
        for (auto& element : m_elements)
        {
            element.offset = m_stride;
            m_stride += element.dataByte;
        }
    }

    std::vector<VertexElement>::iterator begin()
    {
        return m_elements.begin();
    }
    std::vector<VertexElement>::iterator end()
    {
        return m_elements.end();
    }
    std::vector<VertexElement>::const_iterator begin() const
    {
        return m_elements.begin();
    }
    std::vector<VertexElement>::const_iterator end() const
    {
        return m_elements.end();
    }

    GLsizei getStride() const
    {
        return m_stride;
    }

private:
    std::vector<VertexElement> m_elements{};  // 一个顶点的所有元素
    GLsizei m_stride;                         // 两个顶点之间的偏移量
};

class Mesh
{
public:
    Mesh(VertexLayout const& _vertexLayout,
         std::vector<namica::Float> const& _vertices,
         std::vector<namica::UInt> const& _indices)
        : m_vertexLayout{_vertexLayout}, m_indexCount{_indices.size()}
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        GLuint vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(namica::Float) * _vertices.size(),
                     _vertices.data(),
                     GL_STATIC_DRAW);

        // 设置顶点布局
        GLuint index{};
        for (auto const& vertexElement : m_vertexLayout)
        {
            glVertexAttribPointer(index,
                                  vertexElement.dataSize,
                                  vertexElement.dataType,
                                  vertexElement.normalized,
                                  m_vertexLayout.getStride(),
                                  (void*)(uintptr_t)(vertexElement.offset));
            glEnableVertexAttribArray(index++);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(namica::UInt) * _indices.size(),
                     _indices.data(),
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // 禁止拷贝移动
    Mesh(Mesh const&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    void draw()
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    }

private:
    VertexLayout m_vertexLayout{};
    GLuint m_vao{};

    namica::UInt64 m_indexCount{};
};

struct Transform
{
    namica::Vec3 position{};
    namica::Vec3 rotation{};
    namica::Vec3 scale{1.0f};

    namica::Mat4 getTransform() const
    {
        namica::Mat4 model{1.0f};

        model.translate(position);
        model.rotate(namica::radians(rotation.x()), namica::Vec3{1.0f, 0.0f, 0.0f});
        model.rotate(namica::radians(rotation.y()), namica::Vec3{0.0f, 1.0f, 0.0f});
        model.rotate(namica::radians(rotation.z()), namica::Vec3{0.0f, 0.0f, 1.0f});
        model.scale(scale);

        return model;
    }
};

struct CameraData
{
    namica::Float fov{};
    namica::Float aspect{};
    namica::Float zNear{};
    namica::Float zFar{};
};

// 挂载到相机上的
class PlayerController
{
public:
    PlayerController(Transform& cameraTrasf) : m_cameraTrasf(cameraTrasf)
    {
    }

    void init(GLFWwindow* const _window)
    {
        // 注册事件响应回调
        glfwSetWindowUserPointer(_window, this);

        // 键盘回调 void (* GLFWkeyfun)(GLFWwindow* window, int key, int scancode, int action, int
        // mods);
        glfwSetKeyCallback(
            _window, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
                PlayerController* const controller{
                    static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
                controller->onKeyEvent(_key, _scancode, _action, _mods);
            });

        // 鼠标按键回调: void (* GLFWmousebuttonfun)(GLFWwindow* window, int button, int action, int
        // mods);
        glfwSetMouseButtonCallback(
            _window, [](GLFWwindow* _window, int _button, int _action, int _mods) {
                PlayerController* const controller{
                    static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
                controller->onMouseEvent(_button, _action, _mods);
            });

        // 鼠标移动位置回调：typedef void (* GLFWcursorposfun)(GLFWwindow* window, double xpos,
        // double ypos);
        glfwSetCursorPosCallback(_window, [](GLFWwindow* _window, double _xpos, double _ypos) {
            PlayerController* const controller{
                static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
            controller->onMousePosEvent(
                namica::Vec2{static_cast<namica::Float>(_xpos), static_cast<namica::Float>(_ypos)});
        });
    }

    void onKeyEvent(int _key, int _scancode, int _action, int _mods)
    {
        if (_key == GLFW_KEY_A)
        {
            m_moveKeyState[0] = _action != GLFW_RELEASE;
        }
        if (_key == GLFW_KEY_D)
        {
            m_moveKeyState[1] = _action != GLFW_RELEASE;
        }
        if (_key == GLFW_KEY_W)
        {
            m_moveKeyState[2] = _action != GLFW_RELEASE;
        }
        if (_key == GLFW_KEY_S)
        {
            m_moveKeyState[3] = _action != GLFW_RELEASE;
        }
    }

    void onMouseEvent(int _button, int _action, int _mods)
    {
        if (_button == GLFW_MOUSE_BUTTON_LEFT)
        {
            m_mouseLeftState = _action != GLFW_RELEASE;
        }
    }

    void onMousePosEvent(namica::Vec2 const& _pos)
    {
        m_mousePos = _pos;
    }

    void onUpdate(namica::Float const _deltaTime)
    {
        // 计算旋转
        if (m_mouseLeftState)
        {
            namica::Float const deltaX{m_mousePos.x() - m_mousePosOld.x()};
            namica::Float const deltaY{m_mousePos.y() - m_mousePosOld.y()};

            // deltaX 围绕着y轴转, 注意这里给予的值是逆时针方向
            m_cameraTrasf.rotation.y() -= deltaX * m_sensitivity * _deltaTime;
            // deltaY 围绕着x轴转
            m_cameraTrasf.rotation.x() -= deltaY * m_sensitivity * _deltaTime;

            // if (m_cameraTrasf.rotation.y() >= 360.0f || m_cameraTrasf.rotation.y() <= -360.0f)
            // {
            //     m_cameraTrasf.rotation.y() = 0.0f;
            // }
            // if (m_cameraTrasf.rotation.x() >= 360.0f || m_cameraTrasf.rotation.x() <= -360.0f)
            // {
            //     m_cameraTrasf.rotation.x() = 0.0f;
            // }
        }

        // 计算平移

        // 计算当前基于相机transform的right和front方向
        // 计算当前transform的旋转矩阵
        namica::Mat4 rotate{1.0f};
        rotate.rotate(namica::radians(m_cameraTrasf.rotation.x()), namica::Vec3{1.0f, 0.0f, 0.0f});
        rotate.rotate(namica::radians(m_cameraTrasf.rotation.y()), namica::Vec3{0.0f, 1.0f, 0.0f});
        rotate.rotate(namica::radians(m_cameraTrasf.rotation.z()), namica::Vec3{0.0f, 0.0f, 1.0f});

        // 右手坐标系, 拇指为x正轴
        namica::Vec3 right{(rotate * namica::Vec4{1.0f, 0.0f, 0.0f, 0.0f}).normalized()};
        // 同理, 中指为z正轴, 相机是朝向负z轴的
        namica::Vec3 front{(rotate * namica::Vec4{0.0f, 0.0f, -1.0f, 0.0f}).normalized()};

        if (m_moveKeyState[0])  // A
        {
            m_cameraTrasf.position -= right * m_moveSpeed * _deltaTime;
        }
        if (m_moveKeyState[1])  // D
        {
            m_cameraTrasf.position += right * m_moveSpeed * _deltaTime;
        }
        if (m_moveKeyState[2])  // W
        {
            m_cameraTrasf.position += front * m_moveSpeed * _deltaTime;
        }
        if (m_moveKeyState[3])  // S
        {
            m_cameraTrasf.position -= front * m_moveSpeed * _deltaTime;
        }

        // 更新历史鼠标位置
        m_mousePosOld = m_mousePos;
    }

private:
    Transform& m_cameraTrasf;
    namica::Bool m_moveKeyState[4]{
        namica::False, namica::False, namica::False, namica::False};  // A, D, W, S
    namica::Bool m_mouseLeftState{namica::False};
    namica::Vec2 m_mousePosOld{};
    namica::Vec2 m_mousePos{};

    namica::Float m_sensitivity{5.0f};  // 相机旋转灵敏度
    namica::Float m_moveSpeed{1.0f};    // 相机移动速度
};

}  // namespace

TEST_F(TestWindowRender, cube_render)
{
    // 立方体渲染 - 3d渲染

    namica::Vec2 sizeRatio{0.3f, 0.3f};
    namica::Vec4 backgroundColor{0.0f, 0.0f, 0.0f, 1.0f};

    // init
    glfw_opengl::windowRenderInit();
    GLFWwindow* const window{glfw_opengl::createWindow("cub render", sizeRatio.x(), sizeRatio.y())};
    glfw_opengl::renderContextInit(window, true);

    // render data

    // cub相关材质和网格
    // 3d render
    std::string vertexShaderSrc{R"(
        #version 330 core
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec4 color;

        out vec4 vColor;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;

        void main() {
            gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
            vColor = color;
        }
    )"};

    // 2d render
    // std::string vertexShaderSrc{R"(
    //     #version 330 core
    //     layout(location = 0) in vec2 position;
    //     layout(location = 1) in vec4 color;

    //     out vec4 vColor;

    //     uniform mat4 uModel;
    //     uniform mat4 uView;
    //     uniform mat4 uProjection;

    //     void main() {
    //         gl_Position = uProjection * uView * uModel * vec4(position, 0.0, 1.0);
    //         vColor = color;
    //     }
    // )"};

    std::string fragmentShaderSrc{R"(
        #version 330 core

        uniform vec4 uColor;

        in vec4 vColor;

        out vec4 color;

        void main() {
            color = vColor * uColor;
        }
    )"};

    Material material{};
    GLuint const materialShaderProgram{createShaderProgram(vertexShaderSrc, fragmentShaderSrc)};
    material.setShaderProgram(materialShaderProgram);
    material.setParam("uColor", namica::Vec4{1.0f, 1.0f, 1.0f, 1.0f});

    // 3d 立方体
    //       6 ---- 5
    //      /      /|              z y
    //     0 ---- 2 |              |/
    //     | 7    | 4              |-> x
    //     |      |/
    //     1 ---- 3
    //

    std::vector<namica::Float> vertices{
        -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // vertex0
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,  // vertex1
        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex2
        0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex3
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,  // vertex4
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  // vertex5
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex6
        -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex7
    };
    VertexLayout vertexLayout{
        VertexElement{GL_FLOAT, 3},  // pos
        VertexElement{GL_FLOAT, 4},  // color
    };

    std::vector<namica::UInt> indices{
        0, 1, 2, 2, 1, 3,  // face0 front
        2, 3, 5, 5, 3, 4,  // face1 front right
        5, 4, 6, 6, 4, 7,  // face2 back
        6, 7, 0, 0, 7, 1,  // face3 front left
        1, 7, 3, 3, 7, 4,  // face4 bottom
        6, 0, 5, 5, 0, 2,  // face5 top
    };

    // 2d 矩形
    // 0 ----- 2
    // |   /   |
    // 1 ----- 3

    // std::vector<namica::Float> vertices{
    //     -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // vertex0
    //     -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,  // vertex1
    //     0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex2
    //     0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex3
    // };
    // VertexLayout vertexLayout{
    //     VertexElement{GL_FLOAT, 2},  // pos
    //     VertexElement{GL_FLOAT, 4},  // color
    // };
    // std::vector<namica::UInt> indices{0, 1, 2, 2, 1, 3};

    Mesh mesh{vertexLayout, vertices, indices};

    Transform cubTransform{};

    Transform cameraTransform{};
    cameraTransform.position.z() = 2.0f;
    CameraData cameraData{};
    cameraData.fov = namica::radians(60.0f);
    auto windowSize{glfw_opengl::getWindowSize(window)};
    cameraData.aspect = 1.0f * windowSize.first / windowSize.second;
    cameraData.zNear = 0.1f;
    cameraData.zFar = 1000.0f;

    // 获取除开材质之外的uniform location
    GLint const uModelLoc{glGetUniformLocation(materialShaderProgram, "uModel")};
    GLint const uViewLoc{glGetUniformLocation(materialShaderProgram, "uView")};
    GLint const uProjectionLoc{glGetUniformLocation(materialShaderProgram, "uProjection")};

    // namica::Float timeCount{};              // 累计时间
    // namica::Float const rotateSpeed{0.0f};  // 旋转速度 0.5s/ 0.25°
    // namica::Float const rotateTime{0.5f};   // 旋转间隔时间 0.5s

    // 玩家控制器
    PlayerController playerController{cameraTransform};
    playerController.init(window);

    std::chrono::steady_clock::time_point lastPoint{std::chrono::steady_clock::now()};
    while (!glfw_opengl::windowShouldClose(window))
    {
        std::chrono::steady_clock::time_point curPoint{std::chrono::steady_clock::now()};
        namica::Float const delta{
            std::chrono::duration<namica::Float>(curPoint - lastPoint).count()};
        lastPoint = curPoint;

        glfw_opengl::pollEvents();

        // update

        // timeCount += delta;
        // if (timeCount > rotateTime)
        // {
        //     cubTransform.rotation.y() += rotateSpeed;
        //     cubTransform.rotation.x() += rotateSpeed;
        //     if (cubTransform.rotation.y() >= 360.0f)
        //     {
        //         cubTransform.rotation.y() = 0.0f;
        //         cubTransform.rotation.x() = 0.0f;
        //     }
        //     timeCount = 0.0f;
        // }

        playerController.onUpdate(delta);

        // render
        glClearColor(
            backgroundColor.r(), backgroundColor.g(), backgroundColor.b(), backgroundColor.a());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        material.bind();
        // 上传其他uniform
        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, cubTransform.getTransform().data());
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, cameraTransform.getTransform().inversed().data());
        // project
        namica::Mat4 const perspective{namica::Mat4::perspective(
            cameraData.fov, cameraData.aspect, cameraData.zNear, cameraData.zFar)};
        glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, perspective.data());

        mesh.draw();

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}