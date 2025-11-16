#include "namica/renderer/UniformBuffer.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLUniformBuffer.h"

namespace Namica
{

Ref<UniformBuffer> UniformBuffer::create(uint32_t _size, uint32_t _binding)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLUniformBuffer>(_size, _binding);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}
}  // namespace Namica