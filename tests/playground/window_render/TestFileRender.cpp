/**
 * @brief 通过文件IO测试render, 达成游戏引擎的基础底层实现
 *
 */

#include <gtest/gtest.h>
#include "playground/window_render/GlfwOpengl.h"
#include <chrono>
#include <memory>

class TestWindowRender : public testing::Test
{
};

namespace
{

std::shared_ptr<Material> createCubMaterial()
{
    std::shared_ptr<Material> cubMaterial{std::make_shared<Material>()};

    // 顶点着色器
    std::string vertexShaderSRC{R"(
        #version 330 core

        layout(location = 0) in vec3 position;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProject;

        void main(){
            gl_Position = uProject * uView * uModel * vec4(position, 1.0);
        }
    )"};

    // 片段着色器
    std::string fragmentShaderSRC{R"(
        #version 330 core

        out vec4 color;

        uniform vec4 uColor;

        void main(){
            color = uColor;
        }
    )"};

    cubMaterial->setShaderProgram(createShaderProgram(vertexShaderSRC, fragmentShaderSRC));

    return cubMaterial;
}

std::shared_ptr<Mesh> createCubMesh()
{
    // 3d 立方体
    //       6 ---- 5
    //      /      /|              z y
    //     0 ---- 2 |              |/
    //     | 7    | 4              |-> x
    //     |      |/
    //     1 ---- 3
    //

    std::vector<namica::Float> vertices{
        -0.5f, -0.5f, 0.5f,   // vertex0
        -0.5f, -0.5f, -0.5f,  // vertex1
        0.5f,  -0.5f, 0.5f,   // vertex2
        0.5f,  -0.5f, -0.5f,  // vertex3
        0.5f,  0.5f,  -0.5f,  // vertex4
        0.5f,  0.5f,  0.5f,   // vertex5
        -0.5f, 0.5f,  0.5f,   // vertex6
        -0.5f, 0.5f,  -0.5f,  // vertex7
    };
    VertexLayout vertexLayout{VertexElement{GL_FLOAT, 3}};
    std::vector<namica::UInt> indices{
        0, 1, 2, 2, 1, 3,  // face0 front
        2, 3, 5, 5, 3, 4,  // face1 front right
        5, 4, 6, 6, 4, 7,  // face2 back
        6, 7, 0, 0, 7, 1,  // face3 front left
        1, 7, 3, 3, 7, 4,  // face4 bottom
        6, 0, 5, 5, 0, 2,  // face5 top
    };

    std::shared_ptr<Mesh> cubMesh{std::make_shared<Mesh>(vertexLayout, vertices, indices)};
    return cubMesh;
}

class Cub
{
public:
    Cub(std::shared_ptr<Material> const& _material = createCubMaterial(),
        std::shared_ptr<Mesh> const& _mesh = createCubMesh())
        : m_material{_material}, m_mesh{_mesh}
    {
        GLuint const materialShaderProgram{_material->getShaderProgram()};
        m_uModelLoc = {glGetUniformLocation(materialShaderProgram, "uModel")};
        m_uViewLoc = {glGetUniformLocation(materialShaderProgram, "uView")};
        m_uProjectionLoc = {glGetUniformLocation(materialShaderProgram, "uProject")};

        m_material->setParam("uColor", namica::Vec4{1.0f, 1.0f, 1.0f, 1.0f});
    }

    void render(Camera& _camera)
    {
        m_material->bind();
        glUniformMatrix4fv(m_uModelLoc, 1, GL_FALSE, m_transf.getTransform().data());
        glUniformMatrix4fv(m_uViewLoc, 1, GL_FALSE, _camera.getView().data());
        glUniformMatrix4fv(m_uProjectionLoc, 1, GL_FALSE, _camera.getProject().data());

        m_mesh->draw();
    }

    void setColor(namica::Vec4 const _color)
    {
        m_material->setParam("uColor", _color);
    }

private:
    Transform m_transf{};
    std::shared_ptr<Material> m_material{};
    std::shared_ptr<Mesh> m_mesh{};

    GLint m_uModelLoc{};
    GLint m_uViewLoc{};
    GLint m_uProjectionLoc{};
};

}  // namespace

TEST_F(TestWindowRender, file_render)
{
    glfw_opengl::windowRenderInit();
    GLFWwindow* window{glfw_opengl::createWindow("FileRender", 2.0f / 3.0f, 2.0f / 3.0f)};
    glfw_opengl::renderContextInit(window, true);

    namica::Vec4 backgroundColor{0.0f, 0.0f, 0.0f, 1.0f};
    Camera camera{};
    camera.getTransform().position.z() = 4.0f;
    auto windowSize{glfw_opengl::getWindowSize(window)};
    camera.getData().aspect = 1.0f * windowSize.first / windowSize.second;
    PlayerController playerController{camera};
    playerController.init(window);

    Cub cubObj{};

    std::chrono::steady_clock::time_point lastPoint{std::chrono::steady_clock::now()};
    while (!glfw_opengl::windowShouldClose(window))
    {
        std::chrono::steady_clock::time_point point{std::chrono::steady_clock::now()};
        namica::Float delta{std::chrono::duration<namica::Float>(point - lastPoint).count()};
        lastPoint = point;

        glfw_opengl::pollEvents();

        // update
        playerController.onUpdate(delta);

        // render
        glClearColor(
            backgroundColor.r(), backgroundColor.g(), backgroundColor.b(), backgroundColor.a());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubObj.render(camera);

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}