/**
 * @brief 通过文件IO测试render, 达成游戏引擎的基础底层实现
 *
 */

#include <gtest/gtest.h>
#include "playground/window_render/TestRender.h"
#include <chrono>
#include <namica/io/FileSystem.h>
#include <namica/math/Utils.h>

class TestWindowRender : public testing::Test
{
};

namespace
{

// std::shared_ptr<Material> createCubMaterial()
// {
//     namica::FileSystem fileSystem{};
//     // 顶点着色器
//     std::string vertexShaderSRC{
//         fileSystem.loadFileText(std::filesystem::path{NAMICA_ASSETS_DIR} / "shader/cub.vert")};

//     // 片段着色器
//     std::string fragmentShaderSRC{
//         fileSystem.loadFileText(std::filesystem::path{NAMICA_ASSETS_DIR} / "shader/cub.frag")};

//     std::shared_ptr<ShaderProgram> shaderProgram{
//         std::make_shared<ShaderProgram>(vertexShaderSRC, fragmentShaderSRC)};
//     std::shared_ptr<Material> cubMaterial{std::make_shared<Material>(std::move(shaderProgram))};
//     return cubMaterial;
// }

std::shared_ptr<Mesh> createCubMesh(namica::FileSystem& _fileSystem)
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
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,                  // vertex0(0)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,          // vertex1(1)
        0.5f,  -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,          // vertex2(2)
        0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,         // vertex3(3)

        0.5f,  -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,          // vertex4(2)
        0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,         // vertex5(3)
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,          // vertex6(5)
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,         // vertex7(4)

        0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,         // vertex8(5)
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f,0.0f, 0.0f, -1.0f,         // vertex9(4)
        -0.5f, 0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,         // vertex10(6)
        -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,        // vertex11(7)

        -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,     // vertex12(6)
        -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    // vertex13(7)
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,      // vertex14(0)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,    // vertex15(1)

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,    // vertex16(1)
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,     // vertex17(7)
        0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,    // vertex18(3)
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,    // vertex19(4)

        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,       // vertex20(6)
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,      // vertex21(0)
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,      // vertex22(5)
        0.5f,  -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,      // vertex23(2)
    };

    std::vector<namica::UInt> indices{
        0, 1, 2, 2, 1, 3,             // face0 front (0, 1, 2, 2, 1, 3,)
        4, 5, 6, 6, 5, 7,           // face1 front right (2, 3, 5, 5, 3, 4)
        8, 9, 10, 10, 9, 11,    // face2 back (5, 4, 6, 6, 4, 7)
        12, 13, 14, 14, 13, 15, // face3 front left (6, 7, 0, 0, 7, 1)
        16, 17, 18, 18, 17, 19, // face4 bottom (1, 7, 3, 3, 7, 4)
        20, 21, 22, 22, 21, 23, // face5 top (6, 0, 5, 5, 0, 2)
    };

    VertexLayout vertexLayout{
    VertexElement{GL_FLOAT, 3}, 
    VertexElement{GL_FLOAT, 2},
    VertexElement{GL_FLOAT, 3}
    };
    // clang-format on

    MeshPrimitive meshPrimitive{vertexLayout, vertices, indices};
    meshPrimitive.setMaterial(Material::load(_fileSystem, "material/cub_material.json"));
    std::shared_ptr<Mesh> cubMesh{new Mesh{meshPrimitive}};
    return cubMesh;
}

}  // namespace

TEST_F(TestWindowRender, file_render)
{
    glfw_opengl::windowRenderInit();
    GLFWwindow* window{glfw_opengl::createWindow("FileRender", 1.0f / 3.0f, 1.0f / 3.0f)};
    glfw_opengl::renderContextInit(window, true);

    namica::Vec4 backgroundColor{0.0f, 0.0f, 0.0f, 1.0f};
    Camera camera{};
    camera.getTransform().position.z() = 4.0f;
    auto windowSize{glfw_opengl::getWindowSize(window)};
    camera.getData().aspect = 1.0f * windowSize.first / windowSize.second;
    PlayerController playerController{camera};
    playerController.init(window);

    namica::FileSystem fileSystem{};
    fileSystem.setAssetsFolder(NAMICA_ASSETS_DIR);

    Object cubObj{createCubMesh(fileSystem)};
    // cubObj.getMesh()->begin()->getMaterial()->setDoubleSided(namica::False);

    auto littlePrincessMesh{Mesh::load(fileSystem, "models/守护者传说_小公主/小公主.gltf")};
    Object littlePrincessObj{littlePrincessMesh};
    littlePrincessObj.getTransform().position = namica::Vec3{0.0f, -3.9f, -5.0f};
    littlePrincessObj.getTransform().scale *= 0.4f;

    // littlePrincessObj.getMesh()->setDisableDrawIndex(1);
    // littlePrincessObj.getMesh()->setDisableDrawIndex(3);

    auto baizhouziMesh{Mesh::load(fileSystem, "models/蔚蓝档案_白洲梓/白洲梓.gltf")};
    Object baizhouziObj{baizhouziMesh};
    baizhouziObj.getTransform().position = namica::Vec3{-5.0f, -3.9f, -5.0f};
    baizhouziObj.getTransform().scale *= 0.000135f;

    // 漫反射光源
    Light light{};
    light.position = namica::Vec3{0.0f, 1000.0f, 0.0f};

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

        cubObj.onRender(camera, light);
        littlePrincessObj.onRender(camera, light);
        baizhouziObj.onRender(camera, light);

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}
