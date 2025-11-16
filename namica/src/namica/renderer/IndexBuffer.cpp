#include "namica/renderer/IndexBuffer.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLIndexBuffer.h"

namespace Namica
{

Ref<IndexBuffer> IndexBuffer::create(uint32_t const* _indices, uint32_t _count)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLIndexBuffer>(_indices, _count);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica