#include <gtest/gtest.h>
#include "playground/window_render/GlfwOpengl.h"

class TestWindowRender : public testing::Test
{
};

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

    Camera camera{};
    camera.getTransform().position.z() = 2.0f;
    auto windowSize{glfw_opengl::getWindowSize(window)};
    camera.getData().aspect = 1.0f * windowSize.first / windowSize.second;

    // 获取除开材质之外的uniform location
    GLint const uModelLoc{glGetUniformLocation(materialShaderProgram, "uModel")};
    GLint const uViewLoc{glGetUniformLocation(materialShaderProgram, "uView")};
    GLint const uProjectionLoc{glGetUniformLocation(materialShaderProgram, "uProjection")};

    // namica::Float timeCount{};              // 累计时间
    // namica::Float const rotateSpeed{0.0f};  // 旋转速度 0.5s/ 0.25°
    // namica::Float const rotateTime{0.5f};   // 旋转间隔时间 0.5s

    // 玩家控制器
    PlayerController playerController{camera};
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

        // view
        namica::Mat4 viewMat{camera.getView()};
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, viewMat.data());

        // project
        namica::Mat4 const perspective{camera.getProject()};
        glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, perspective.data());

        mesh.draw();

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}