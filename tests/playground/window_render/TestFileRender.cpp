/**
 * @brief 通过文件IO测试render, 达成游戏引擎的基础底层实现
 *
 */

#include <gtest/gtest.h>
#include "playground/window_render/GlfwOpengl.h"
#include <chrono>
#include <namica/io/FileSystem.h>

class TestWindowRender : public testing::Test
{
};

namespace
{

std::shared_ptr<Material> createCubMaterial()
{
    // 顶点着色器
    std::string vertexShaderSRC{R"(
        #version 330 core

        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 uv;

        out vec2 vUV;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProject;

        void main(){
            gl_Position = uProject * uView * uModel * vec4(position, 1.0);
            vUV = uv;
        }
    )"};

    // 片段着色器
    std::string fragmentShaderSRC{R"(
        #version 330 core

        in vec2 vUV;

        out vec4 color;

        uniform vec4 uColor;
        uniform sampler2D uTexture;

        void main(){
            vec4 texColor = texture(uTexture, vUV);
            color = texColor * uColor;
        }
    )"};

    std::shared_ptr<ShaderProgram> shaderProgram{
        std::make_shared<ShaderProgram>(vertexShaderSRC, fragmentShaderSRC)};
    std::shared_ptr<Material> cubMaterial{std::make_shared<Material>(std::move(shaderProgram))};
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

    // -0.5f, -0.5f, 0.5f   // vertex0
    // -0.5f, -0.5f, -0.5f  // vertex1
    // 0.5f,  -0.5f, 0.5f   // vertex2
    // 0.5f,  -0.5f, -0.5f  // vertex3
    // 0.5f,  0.5f,  -0.5f  // vertex4
    // 0.5f,  0.5f,  0.5f   // vertex5
    // -0.5f, 0.5f,  0.5f   // vertex6
    // -0.5f, 0.5f,  -0.5f  // vertex7

    // 贴图
    //     (0,1)----(1,1)
    //       |   /    |
    //     (0,0)----(1,0)

    // clang-format off
    std::vector<namica::Float> vertices{
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,           // vertex0(0)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,         // vertex1(1)
        0.5f,  -0.5f, 0.5f, 1.0f, 1.0f,      // vertex2(2)
        0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,     // vertex3(3)

        0.5f,  -0.5f, 0.5f, 0.0f, 1.0f,      // vertex4(2)
        0.5f,  -0.5f, -0.5f, 0.0f, 0.0f,     // vertex5(3)
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,      // vertex6(5)
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f,     // vertex7(4)

        0.5f,  0.5f,  0.5f, 0.0f, 1.0f,      // vertex8(5)
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f,     // vertex9(4)
        -0.5f, 0.5f,  0.5f, 1.0f, 1.0f,      // vertex10(6)
        -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f,     // vertex11(7)

        -0.5f, 0.5f,  0.5f, 0.0f, 1.0f,      // vertex12(6)
        -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f,     // vertex13(7)
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f,      // vertex14(0)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,     // vertex15(1)

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,     // vertex16(1)
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,      // vertex17(7)
        0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,     // vertex18(3)
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f,     // vertex19(4)

        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // vertex20(6)
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // vertex21(0)
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // vertex22(5)
        0.5f,  -0.5f, 0.5f, 1.0f, 0.0f, // vertex23(2)
    };

    std::vector<namica::UInt> indices{
        0, 1, 2, 2, 1, 3,             // face0 front (0, 1, 2, 2, 1, 3,)
        4, 5, 6, 6, 5, 7,           // face1 front right (2, 3, 5, 5, 3, 4)
        8, 9, 10, 10, 9, 11,    // face2 back (5, 4, 6, 6, 4, 7)
        12, 13, 14, 14, 13, 15, // face3 front left (6, 7, 0, 0, 7, 1)
        16, 17, 18, 18, 17, 19, // face4 bottom (1, 7, 3, 3, 7, 4)
        20, 21, 22, 22, 21, 23, // face5 top (6, 0, 5, 5, 0, 2)
    };
    // clang-format on
    VertexLayout vertexLayout{VertexElement{GL_FLOAT, 3}, VertexElement{GL_FLOAT, 2}};

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
        m_material->setParam("uColor", namica::Vec4{1.0f, 1.0f, 1.0f, 1.0f});
    }

    void render(Camera& _camera)
    {
        m_material->bind();
        ShaderProgram& shaderProgram{m_material->getShaderProgram()};
        shaderProgram.setParam("uModel", m_transf.getTransform());
        shaderProgram.setParam("uView", _camera.getView());
        shaderProgram.setParam("uProject", _camera.getProject());

        m_mesh->draw();
    }

    void setColor(namica::Vec4 const _color)
    {
        m_material->setParam("uColor", _color);
    }

    Material& getMaterial()
    {
        return *m_material;
    }

private:
    Transform m_transf{};
    std::shared_ptr<Material> m_material{};
    std::shared_ptr<Mesh> m_mesh{};
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

    namica::FileSystem fileSystem{};
    fileSystem.setAssetsFolder(NAMICA_ASSETS_DIR);

    namica::Int textureWidth{};
    namica::Int textureHeight{};
    namica::Int textureChannels{};
    auto textureBuffer{
        fileSystem.loadAssetImage("image/木板.jpg", textureWidth, textureHeight, textureChannels)};
    std::shared_ptr<Texture> texture{};
    if (!textureBuffer.empty())
    {
        std::cout << "已加载图片: 木板.jpg" << std::endl;
        std::cout << "宽度: " << textureWidth << std::endl;
        std::cout << "高度: " << textureHeight << std::endl;
        std::cout << "通道数: " << textureChannels << std::endl;

        texture = std::make_shared<Texture>(textureWidth, textureHeight, textureBuffer.data());
        cubObj.getMaterial().setParam("uTexture", texture);
    }

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