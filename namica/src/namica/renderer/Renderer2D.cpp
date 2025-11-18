#include "namica/renderer/Renderer2D.h"
#include "namica/renderer/RendererConfig.h"
#include "namica/renderer/VertexArray.h"
#include "namica/renderer/UniformBuffer.h"
#include "namica/renderer/Shader.h"
#include "namica/renderer/Texture.h"
#include "namica/renderer/VertexBuffer.h"
#include "namica/renderer/IndexBuffer.h"
#include "namica/renderer/RendererCommand.h"

#include <array>

namespace Namica
{

// 四边形顶点数据
struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    int TexId;
    float TilingFactor;

    int EntityID;
};

// 圆顶点数据
struct CircleVertex
{
    glm::vec3 WorldPosition;
    glm::vec3 LocalPosition;  // 局部世界坐标, 类似于UV?
    glm::vec4 Color;
    float Thickness;
    float Fade;

    int EntityID;
};

// 线段顶点数据
struct LineVertex
{
    glm::vec3 Position;
    glm::vec4 Color;

    int EntityID;
};

struct Renderer2DData
{
    uint32_t maxQuads{10000};               // 允许的最大四边形绘制数量
    uint32_t maxVertices{maxQuads * 4};     // 允许的最大绘制顶点个数
    uint32_t maxIndices{maxQuads * 3 * 2};  // 允许的最大绘制索引个数
    static int const maxTextureSolts{32};   // 允许的最大绑定纹理槽数量

    // 四边形渲染
    Ref<VertexArray> quadVertexArray{nullptr};    // 四边形的基础顶点数组对象
    Ref<VertexBuffer> quadVertexBuffer{nullptr};  // 四边形的顶点缓冲区
    Ref<Shader> quadTextureShader{nullptr};       // 可渲染纹理的四边形shader
    Ref<Texture2D> whiteTexure{nullptr};          // 不渲染图片的清空下使用白色纹理

    QuadVertex* quadVertexBufferBase{nullptr};  // 顶点数组实际准备数据
    QuadVertex* quadVertexBufferPtr{nullptr};   // 顶点数组当前顶点数据
    uint32_t quadCount{0};                      // 四边形绘制个数

    std::array<Ref<Texture2D>, maxTextureSolts> quadTextureSolts;
    int quadTextureSoltCount = 0;  // 四边形槽占用数

    // Circle
    Ref<VertexArray> circleVertexArray{nullptr};
    Ref<VertexBuffer> circleVertexBuffer{nullptr};
    Ref<Shader> circleShader{nullptr};

    CircleVertex* circleVertexBufferBase{nullptr};
    CircleVertex* circleVertexBufferPtr{nullptr};
    uint32_t circleCount{0};

    // line
    Ref<VertexArray> lineVertexArray{nullptr};
    Ref<VertexBuffer> lineVertexBuffer{nullptr};
    Ref<Shader> lineShader{nullptr};

    LineVertex* lineVertexBufferBase{nullptr};
    LineVertex* lineVertexBufferPtr{nullptr};
    uint32_t lineCount{0};

    float lineWidth = 2.0f;

    // 其他辅助数据
    Renderer2D::Statistics stats;

    glm::vec4 const refPositions[4] = {
        glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f},
        glm::vec4{0.5f, -0.5f, 0.0f, 1.0f},
        glm::vec4{0.5f, 0.5f, 0.0f, 1.0f},
        glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f}};  // 参照坐标, 坐标的transform依据此进行变换
    glm::vec2 const refTexCoords[4] = {glm::vec2{0.0f, 0.0f},
                                       glm::vec2{1.0f, 0.0f},
                                       glm::vec2{1.0f, 1.0f},
                                       glm::vec2{0.0f, 1.0f}};  // 参照UV坐标

    // uniform缓冲区
    struct CameraData
    {
        glm::mat4 projectionView;
    };
    CameraData cameraBuffer;
    Ref<UniformBuffer> cameraUniformBuffer;
};

static Renderer2DData s_data{};

void Renderer2D::init(Renderer2DConfig _renderer2DConfig)
{
    s_data.maxQuads = _renderer2DConfig.maxQuads;

    // 四边形纹理渲染初始化
    // 1. 创建quad顶点数组对象
    s_data.quadVertexArray = VertexArray::create();

    // 2. 创建quad固定大小, 非固定数据顶点缓冲区
    s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(QuadVertex));
    // 3. quad设置布局 -> vertex shader如何读取顶点数据
    s_data.quadVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"},
                                        {ShaderDataType::Float4, "a_Color"},
                                        {ShaderDataType::Float2, "a_TexCoord"},
                                        {ShaderDataType::Int, "a_TexId"},
                                        {ShaderDataType::Float, "a_TilingFactor"},
                                        {ShaderDataType::Int, "a_EntityID"}});
    // 4. 设置到顶点数组对象中去
    s_data.quadVertexArray->setVertexBuffer(s_data.quadVertexBuffer);

    // 创建顶点数据
    s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

    // 5. 创建索引数组, 标识顶点读取顺序
    uint32_t* indexBuffer = new uint32_t[s_data.maxIndices];
    // 设置四边形的索引顺序
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_data.maxIndices; i += 6)
    {
        indexBuffer[i + 0] = offset + 0;
        indexBuffer[i + 1] = offset + 1;
        indexBuffer[i + 2] = offset + 2;

        indexBuffer[i + 3] = offset + 2;
        indexBuffer[i + 4] = offset + 3;
        indexBuffer[i + 5] = offset + 0;
        offset += 4;
    }
    auto quadIndexBuffer = IndexBuffer::create(indexBuffer, s_data.maxIndices);
    // 6. 设置到到顶点数组对象中去
    s_data.quadVertexArray->setIndexBuffer(quadIndexBuffer);

    // 7. 读取纹理shader
    s_data.quadTextureShader = Shader::create("assets/shaders/Renderer2D_QuadTexture.glsl");

    // 8. 创建单位白色纹理, 用于渲染纯色方块
    s_data.whiteTexure = Texture2D::create(1, 1);
    uint32_t data = 0xffffffff;  // 纯白
    s_data.whiteTexure->setData(&data, sizeof(data));

    s_data.quadTextureSolts[0] = s_data.whiteTexure;  // 0号位绑定纯白纹理
    s_data.quadTextureSoltCount = 1;

    int textureSolts[s_data.maxTextureSolts];
    for (uint32_t i{0}; i < s_data.maxTextureSolts; ++i)
    {
        textureSolts[i] = i;
    }
    s_data.quadTextureShader->setIntArray("u_Textures", textureSolts, s_data.maxTextureSolts);

    // Circle渲染初始化
    s_data.circleVertexArray = VertexArray::create();

    // TODO: 圆的绘制上限使用MaxQuads, 因为原理均为绘制两个三角形
    s_data.circleVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(CircleVertex));

    s_data.circleVertexBuffer->setLayout({{ShaderDataType::Float3, "a_WorldPosition"},
                                          {ShaderDataType::Float3, "a_LocalPosition"},
                                          {ShaderDataType::Float4, "a_Color"},
                                          {ShaderDataType::Float, "a_Thickness"},
                                          {ShaderDataType::Float, "a_Fade"},
                                          {ShaderDataType::Int, "a_EntityID"}});
    s_data.circleVertexArray->setVertexBuffer(s_data.circleVertexBuffer);

    s_data.circleVertexBufferBase = new CircleVertex[s_data.maxVertices];

    auto circleIndexBuffer = IndexBuffer::create(indexBuffer, s_data.maxIndices);
    s_data.circleVertexArray->setIndexBuffer(circleIndexBuffer);

    s_data.circleShader = Shader::create("assets/shaders/Renderer2D_Circle.glsl");

    // line 渲染初始化
    s_data.lineVertexArray = VertexArray::create();

    // TODO: 线的绘制上限借用MaxQuads
    s_data.lineVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(LineVertex));

    s_data.lineVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"},
                                        {ShaderDataType::Float4, "a_Color"},
                                        {ShaderDataType::Int, "a_EntityID"}});
    s_data.lineVertexArray->setVertexBuffer(s_data.lineVertexBuffer);

    s_data.lineVertexBufferBase = new LineVertex[s_data.maxVertices];

    s_data.lineShader = Shader::create("assets/shaders/Renderer2D_Line.glsl");
    // 设置绘制线段的宽度
    RendererCommand::setLineWidth(s_data.lineWidth);

    s_data.cameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);

    delete[] indexBuffer;
    indexBuffer = nullptr;
}

void Renderer2D::shutdown()
{
    delete[] s_data.quadVertexBufferBase;
    s_data.quadVertexBufferBase = s_data.quadVertexBufferPtr = nullptr;

    delete[] s_data.circleVertexBufferBase;
    s_data.circleVertexBufferBase = s_data.circleVertexBufferPtr = nullptr;

    delete[] s_data.lineVertexBufferBase;
    s_data.lineVertexBufferBase = s_data.lineVertexBufferPtr = nullptr;
}

// 重置绘制
static inline void startBatch()
{
    // 四边形绘制重置
    s_data.quadCount = 0;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

    // 绑定纹理槽重置
    s_data.quadTextureSoltCount = 1;  // 0号始终由白色纹理占用

    // 圆形绘制重置
    s_data.circleCount = 0;
    s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;

    // 线段绘制重置
    s_data.lineCount = 0;
    s_data.lineVertexBufferPtr = s_data.lineVertexBufferBase;
}

void Renderer2D::beginScene(glm::mat4 const& _pv)
{
    s_data.cameraBuffer.projectionView = _pv;
    s_data.cameraUniformBuffer->setData(&s_data.cameraBuffer, sizeof(Renderer2DData::cameraBuffer));

    startBatch();
}

// 绘制数据
static void flush()
{
    if (s_data.quadCount > 0)
    {
        // 上传顶点数据
        uint32_t size = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr -
                                   (uint8_t*)s_data.quadVertexBufferBase);
        s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, size);

        // 绑定纹理槽
        for (int i{0}; i < s_data.quadTextureSoltCount; ++i)
        {
            s_data.quadTextureSolts[i]->bind(i);
        }

        RendererCommand::drawIndexed(
            s_data.quadVertexArray, s_data.quadTextureShader, s_data.quadCount * 6);
        s_data.stats.drawCalls++;
    }

    if (s_data.circleCount > 0)
    {
        uint32_t size = (uint32_t)((uint8_t*)s_data.circleVertexBufferPtr -
                                   (uint8_t*)s_data.circleVertexBufferBase);
        s_data.circleVertexBuffer->setData(s_data.circleVertexBufferBase, size);

        RendererCommand::drawIndexed(
            s_data.circleVertexArray, s_data.circleShader, s_data.circleCount * 6);

        s_data.stats.drawCalls++;
    }

    if (s_data.lineCount > 0)
    {
        uint32_t size = (uint32_t)((uint8_t*)s_data.lineVertexBufferPtr -
                                   (uint8_t*)s_data.lineVertexBufferBase);
        s_data.lineVertexBuffer->setData(s_data.lineVertexBufferBase, size);

        RendererCommand::drawLines(s_data.lineVertexArray, s_data.lineShader, s_data.lineCount * 2);
        s_data.stats.drawCalls++;
    }
}

void Renderer2D::endScene()
{
    flush();
}

void Renderer2D::flushAndReset()
{
    flush();
    startBatch();
}

void Renderer2D::drawQuad(glm::mat4 const& _transform, glm::vec4 const& _color, int _entityID)
{
    if (s_data.quadCount >= s_data.maxQuads)
    {
        // 超出限制, 重置flush
        flushAndReset();
    }

    int const whiteTexureId = 0;
    float const tilingFactor = 1.0f;
    // 多批次渲染, 准备数据
    // 3 ----- 2
    // |   /   |
    // 0 ----- 1

    for (int i = 0; i < 4; ++i)
    {
        s_data.quadVertexBufferPtr->Position = _transform * s_data.refPositions[i];
        s_data.quadVertexBufferPtr->Color = _color;
        s_data.quadVertexBufferPtr->TexCoord = s_data.refTexCoords[i];
        s_data.quadVertexBufferPtr->TexId = whiteTexureId;
        s_data.quadVertexBufferPtr->TilingFactor = tilingFactor;
        s_data.quadVertexBufferPtr->EntityID = _entityID;
        s_data.quadVertexBufferPtr++;
    }

    s_data.quadCount++;
    s_data.stats.quadCount++;
}

void Renderer2D::drawQuad(glm::mat4 const& _transform,
                          Ref<Texture2D> const& _texture,
                          glm::vec4 const& _tintColor,
                          float _tilingFactor,
                          int _entityID)
{
    if (s_data.quadCount >= s_data.maxQuads)
    {
        // 超出限制, 重置flush
        flushAndReset();
    }

    // 寻找纹理槽index
    int texureID = 0;
    for (int i{1}; i < s_data.quadTextureSoltCount; ++i)
    {
        if (s_data.quadTextureSolts[i]->isEqual(*_texture))
        {
            texureID = i;
            break;
        }
    }
    if (texureID == 0)
    {
        if (s_data.quadTextureSoltCount >= s_data.maxTextureSolts)
        {
            flushAndReset();
        }
        texureID = s_data.quadTextureSoltCount;
        s_data.quadTextureSolts[s_data.quadTextureSoltCount++] = _texture;
    }

    for (int i = 0; i < 4; ++i)
    {
        s_data.quadVertexBufferPtr->Position = _transform * s_data.refPositions[i];
        s_data.quadVertexBufferPtr->Color = _tintColor;
        s_data.quadVertexBufferPtr->TexCoord = s_data.refTexCoords[i];
        s_data.quadVertexBufferPtr->TexId = texureID;
        s_data.quadVertexBufferPtr->TilingFactor = _tilingFactor;
        s_data.quadVertexBufferPtr->EntityID = _entityID;
        s_data.quadVertexBufferPtr++;
    }
    s_data.quadCount++;
    s_data.stats.quadCount++;
}

void Renderer2D::drawCircle(glm::mat4 const& _transform,
                            glm::vec4 const& _color,
                            float _thickness,
                            float _fade,
                            int _entityID)
{
    if (s_data.circleCount >= s_data.maxQuads)
    {
        flushAndReset();
    }

    for (int i = 0; i < 4; ++i)
    {
        s_data.circleVertexBufferPtr->WorldPosition = _transform * s_data.refPositions[i];
        s_data.circleVertexBufferPtr->LocalPosition = 2.0f * s_data.refPositions[i];
        s_data.circleVertexBufferPtr->Color = _color;
        s_data.circleVertexBufferPtr->Thickness = _thickness;
        s_data.circleVertexBufferPtr->Fade = _fade;
        s_data.circleVertexBufferPtr->EntityID = _entityID;
        s_data.circleVertexBufferPtr++;
    }
    s_data.circleCount++;
    s_data.stats.quadCount++;
}

void Renderer2D::drawLine(glm::vec3 const& _p0,
                          glm::vec3 const& _p1,
                          glm::vec4 const& _color,
                          int _entityID)
{
    if (s_data.lineCount >= s_data.maxQuads * 2)
    {
        flushAndReset();
    }

    s_data.lineVertexBufferPtr->Position = _p0;
    s_data.lineVertexBufferPtr->Color = _color;
    s_data.lineVertexBufferPtr->EntityID = _entityID;
    s_data.lineVertexBufferPtr++;

    s_data.lineVertexBufferPtr->Position = _p1;
    s_data.lineVertexBufferPtr->Color = _color;
    s_data.lineVertexBufferPtr->EntityID = _entityID;
    s_data.lineVertexBufferPtr++;

    s_data.lineCount++;
    s_data.stats.lineCount++;
}

void Renderer2D::drawRect(glm::vec3 const& _position,
                          glm::vec2 const& _halfSize,
                          glm::vec4 const& _color,
                          int _entityID)
{
    glm::vec3 p0{_position.x - _halfSize.x, _position.y - _halfSize.y, _position.z};
    glm::vec3 p1{_position.x + _halfSize.x, _position.y - _halfSize.y, _position.z};
    glm::vec3 p2{_position.x + _halfSize.x, _position.y + _halfSize.y, _position.z};
    glm::vec3 p3{_position.x - _halfSize.x, _position.y + _halfSize.y, _position.z};

    drawLine(p0, p1, _color, _entityID);
    drawLine(p1, p2, _color, _entityID);
    drawLine(p2, p3, _color, _entityID);
    drawLine(p3, p0, _color, _entityID);
}

void Renderer2D::drawRect(glm::mat4 const& _transform, glm::vec4 const& _color, int _entityID)
{
    glm::vec3 p0 = _transform * s_data.refPositions[0];
    glm::vec3 p1 = _transform * s_data.refPositions[1];
    glm::vec3 p2 = _transform * s_data.refPositions[2];
    glm::vec3 p3 = _transform * s_data.refPositions[3];

    drawLine(p0, p1, _color, _entityID);
    drawLine(p1, p2, _color, _entityID);
    drawLine(p2, p3, _color, _entityID);
    drawLine(p3, p0, _color, _entityID);
}

Renderer2D::Statistics Renderer2D::getStats()
{
    return s_data.stats;
}

void Renderer2D::resetStats()
{
    s_data.stats.drawCalls = 0;
    s_data.stats.quadCount = 0;
    s_data.stats.lineCount = 0;
}

}  // namespace Namica