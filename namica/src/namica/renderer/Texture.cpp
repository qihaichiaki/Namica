#include "namica/renderer/Texture.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLTexture2D.h"

namespace Namica
{

Ref<Texture2D> Texture2D::create(uint32_t _width, uint32_t _height)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLTexture2D>(_width, _height);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

Ref<Texture2D> Texture2D::create(std::string const& _path)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLTexture2D>(_path);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica