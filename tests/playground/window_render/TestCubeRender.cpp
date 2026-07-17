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
        model.rotate(rotation.x(), namica::Vec3{1.0f, 0.0f, 0.0f});
        model.rotate(rotation.y(), namica::Vec3{0.0f, 1.0f, 0.0f});
        model.rotate(rotation.z(), namica::Vec3{0.0f, 0.0f, 1.0f});
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

}  // namespace

TEST_F(TestWindowRender, cube_render)
{
    // 立方体渲染 - 3d渲染

    namica::Vec2 sizeRatio{0.3f, 0.3f};
    namica::Vec4 backgroundColor{0.961f, 0.961f, 0.863f, 1.0f};

    // init
    glfw_opengl::windowRenderInit();
    GLFWwindow* const window{glfw_opengl::createWindow("cub render", sizeRatio.x(), sizeRatio.y())};
    glfw_opengl::renderContextInit(window);

    // render data

    // 3d render
    // std::string vertexShaderSrc{R"(
    //     #version 330 core
    //     layout(location = 0) in vec3 position;
    //     layout(location = 1) in vec4 color;

    //     out vec4 vColor;

    //     void main() {
    //         gl_Position = vec4(position, 1.0);
    //         vColor = color;
    //     }
    // )"};

    // 2d render
    std::string vertexShaderSrc{R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec4 color;

        out vec4 vColor;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;

        void main() {
            gl_Position = uProjection * uView * uModel * vec4(position, -1.0, 1.0);
            vColor = color;
        }
    )"};

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

    // std::vector<namica::Float> vertices{
    //     -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // vertex0
    //     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,  // vertex1
    //     0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex2
    //     0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex3
    //     0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,  // vertex4
    //     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  // vertex5
    //     -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex6
    //     -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex7
    // };
    // VertexLayout vertexLayout{
    //     VertexElement{GL_FLOAT, 3},  // pos
    //     VertexElement{GL_FLOAT, 4},  // color
    // };

    // std::vector<namica::UInt> indices{
    //     0, 1, 2, 2, 1, 3,  // face0 front
    //     2, 3, 5, 5, 3, 4,  // face1 front right
    //     5, 4, 6, 6, 4, 7,  // face2 back
    //     6, 7, 0, 0, 7, 1,  // face3 front left
    //     1, 7, 3, 3, 7, 4,  // face4 bottom
    //     6, 0, 5, 5, 0, 2,  // face5 top
    // };

    // std::vector<namica::UInt> indices{
    //     0, 1, 2, 2, 1, 3,  // face0 front
    //     2, 3, 5, 5, 3, 4,  // face1 front right
    //     5, 4, 6, 6, 4, 7,  // face2 back
    //     6, 7, 0, 0, 7, 1,  // face3 front left
    //     1, 7, 3, 3, 7, 4,  // face4 bottom
    //     6, 0, 5, 5, 0, 2,  // face5 top
    // };

    // 2d 矩形
    // 0 ----- 2
    // |   /   |
    // 1 ----- 3

    std::vector<namica::Float> vertices{
        -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  // vertex0
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,  // vertex1
        0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  // vertex2
        0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  // vertex3
    };
    VertexLayout vertexLayout{
        VertexElement{GL_FLOAT, 2},  // pos
        VertexElement{GL_FLOAT, 4},  // color
    };
    std::vector<namica::UInt> indices{0, 1, 2, 2, 1, 3};

    Mesh mesh{vertexLayout, vertices, indices};

    Transform cubTransform{};

    Transform cameraTransform{};
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

    while (!glfw_opengl::windowShouldClose(window))
    {
        glfw_opengl::pollEvents();

        // render
        glClearColor(
            backgroundColor.r(), backgroundColor.g(), backgroundColor.b(), backgroundColor.a());
        glClear(GL_COLOR_BUFFER_BIT);

        material.bind();
        // 上传其他uniform
        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, cubTransform.getTransform().data());
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, cameraTransform.getTransform().inversed().data());
        // project
        namica::Mat4 const perspective{namica::perspective(
            cameraData.fov, cameraData.aspect, cameraData.zNear, cameraData.zFar)};
        glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, perspective.data());

        mesh.draw();

        glfw_opengl::swapBuffers(window);
    }

    glfw_opengl::windowShutdown();
}