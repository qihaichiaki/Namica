#include "namica/imgui/ImGuiImpl.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLImGuiImpl.h"

namespace Namica
{

Scope<ImGuiImplRenderer> ImGuiImplRenderer::create()
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createScope<OpenGLImGuiImpl>();
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}
}  // namespace Namica