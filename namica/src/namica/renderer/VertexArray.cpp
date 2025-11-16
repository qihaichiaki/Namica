#include "namica/renderer/VertexArray.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLVertexArray.h"

namespace Namica
{

Ref<VertexArray> VertexArray::create()
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLVertexArray>();
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica