#pragma once

#include <utility>
#include <string_view>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <unordered_set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <namica/Common.h>
#include <namica/math/Vector.h>
#include <namica/math/Quaternion.h>
#include <namica/math/Matrix.h>
#include <namica/math/Utils.h>

struct GLFWwindow;

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

namespace namica
{
class FileSystem;
}

class Texture
{
public:
    Texture(namica::Int const _width, namica::Int const _height, namica::UChar const* _data);
    ~Texture();
    void bind();

    static std::shared_ptr<Texture> load(namica::FileSystem& _fileSystem,
                                         std::filesystem::path const& _textureAssetPath);

private:
    GLuint m_textureObj{};
};

class ShaderProgram
{
public:
    ShaderProgram(std::string const& _vertexShaderSrc, std::string const& _fragmentShaderSrc);
    ~ShaderProgram();

    void bind();

    // 下面的设置参数值必须在绑定之后设置
    void setParam(std::string const& _id, namica::Float const& _value);
    void setParam(std::string const& _id, namica::Vec2 const& _value);
    void setParam(std::string const& _id, namica::Vec3 const& _value);
    void setParam(std::string const& _id, namica::Vec4 const& _value);
    void setParam(std::string const& _id, namica::Mat4 const& _value);
    void setParam(std::string const& _id, Texture* _value);

private:
    GLint getUniformLocation(std::string const& _id);

private:
    GLuint m_shaderProgram{};

    std::unordered_map<std::string, GLint> m_uniformLocation{};
    namica::UInt m_curTextureIndex{};
};

// 材质: shader program + uniform
class Material
{
public:
    Material(std::shared_ptr<ShaderProgram> const& _shaderProgram);
    std::shared_ptr<ShaderProgram> getShaderProgram();

    void setParam(std::string const& _id, namica::Float const& _value);
    void setParam(std::string const& _id, namica::Vec2 const& _value);
    void setParam(std::string const& _id, namica::Vec3 const& _value);
    void setParam(std::string const& _id, namica::Vec4 const& _value);
    void setParam(std::string const& _id, std::shared_ptr<Texture> const& _value);

    void bind();

    static std::shared_ptr<Material> load(namica::FileSystem& _fileSystem,
                                          std::filesystem::path const& _materialAssetPath);

private:
    std::shared_ptr<ShaderProgram> m_shaderProgram{};
    std::unordered_map<std::string, namica::Float> m_floatData{};
    std::unordered_map<std::string, namica::Vec2> m_vec2Data{};
    std::unordered_map<std::string, namica::Vec3> m_vec3Data{};
    std::unordered_map<std::string, namica::Vec4> m_vec4Data{};
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureData{};
};

// mesh, VBO + EBO + VAO

// 顶点元素
struct VertexElement
{
    namica::UInt index{0};
    GLenum dataType{GL_FLOAT};
    GLint dataSize{0};
    GLboolean normalized{GL_FALSE};
    GLsizei dataByte{};
    namica::UInt offset{};

    VertexElement() = default;
    VertexElement(GLenum dataType, GLint _dataSize);
};

// 顶点布局
class VertexLayout
{
public:
    VertexLayout() = default;
    VertexLayout(std::initializer_list<VertexElement> const& _elements);

    void push(VertexElement const& _element);

    GLsizei getStride() const;

    std::vector<VertexElement>::iterator begin();
    std::vector<VertexElement>::iterator end();
    std::vector<VertexElement>::const_iterator begin() const;
    std::vector<VertexElement>::const_iterator end() const;

private:
    std::vector<VertexElement> m_elements{};  // 一个顶点的所有元素
    GLsizei m_stride;                         // 两个顶点之间的偏移量
};

struct Transform
{
    namica::Vec3 position{};
    // namica::Vec3 rotation{};
    namica::Quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    namica::Vec3 scale{1.0f};

    namica::Mat4 getTransform() const;
};

struct CameraData
{
    namica::Float fov{namica::radians(60.0f)};
    namica::Float aspect{};
    namica::Float zNear{0.1f};
    namica::Float zFar{1000.0f};
};

class Camera
{
public:
    Transform& getTransform();
    CameraData& getData();

    namica::Mat4 getView() const;
    namica::Mat4 getProject() const;

private:
    Transform m_trasf{};
    CameraData m_data{};
};

// 挂载到相机上的
class PlayerController
{
public:
    PlayerController(Camera& camera);
    void init(GLFWwindow* const _window);
    void onUpdate(namica::Float const _deltaTime);

private:
    void onKeyEvent(int _key, int _scancode, int _action, int _mods);
    void onMouseEvent(int _button, int _action, int _mods);
    void onMousePosEvent(namica::Vec2 const& _pos);

private:
    Camera& m_camera;
    namica::Bool m_moveKeyState[4]{
        namica::False, namica::False, namica::False, namica::False};  // A, D, W, S
    namica::Bool m_mouseLeftState{namica::False};
    namica::Vec2 m_mousePosOld{};
    namica::Vec2 m_mousePos{};

    namica::Float m_sensitivity{0.5f};  // 相机旋转灵敏度
    namica::Float m_moveSpeed{1.0f};    // 相机移动速度
};

// mesh中的最小渲染单元 -> VAO + material
class MeshPrimitive
{
public:
    MeshPrimitive() = default;
    MeshPrimitive(VertexLayout const& _vertexLayout,
                  std::vector<namica::Float> const& _vertices,
                  std::vector<namica::UInt> const& _indices);

    MeshPrimitive(VertexLayout const& _vertexLayout, std::vector<namica::Float> const& _vertices);

    void setMaterial(std::shared_ptr<Material> const& _material);
    std::shared_ptr<Material> getMaterial();

    void draw() const;

private:
    void init(VertexLayout const& _vertexLayout,
              std::vector<namica::Float> const& _vertices,
              std::vector<namica::UInt> const& _indices);

private:
    GLuint m_vao{};
    std::shared_ptr<Material> m_material{nullptr};

    namica::UInt64 m_indexCount{};
    namica::UInt64 m_vertexCount{};
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(std::initializer_list<MeshPrimitive> const& _meshPrimitives);

    // 禁止拷贝移动
    Mesh(Mesh const&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    std::vector<MeshPrimitive>::iterator begin();
    std::vector<MeshPrimitive>::iterator end();
    std::vector<MeshPrimitive>::const_iterator begin() const;
    std::vector<MeshPrimitive>::const_iterator end() const;

    void pushPrimitive(MeshPrimitive const& _meshPrimitive);

    void draw() const;

    static std::shared_ptr<Mesh> load(namica::FileSystem& _fileSystem,
                                      std::filesystem::path const& _meshAssetPath);

private:
    std::vector<MeshPrimitive> m_meshPrimitives{};
};

class Object
{
public:
    Object(std::shared_ptr<Mesh> const& _mesh);

    std::shared_ptr<Mesh> getMesh();
    Transform& getTransform();

    void onRender(Camera& _camera);

private:
    Transform m_transf{};
    std::shared_ptr<Mesh> m_mesh{};
    std::unordered_set<ShaderProgram*> m_ShaderPrograms{};
};