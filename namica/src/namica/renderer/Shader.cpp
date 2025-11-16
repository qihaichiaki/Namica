#include "namica/renderer/Shader.h"
#include "namica/renderer/RendererCommand.h"
#include "namica/core/Log.h"

#include "platform/opengl/OpenGLShader.h"

namespace Namica
{

Ref<Shader> Shader::create(std::string_view _filePath)
{
    switch (RendererCommand::getRendererAPIType())
    {
        case RendererAPIType::OpenGL:
            return createRef<OpenGLShader>(_filePath);
        default:
            NAMICA_CORE_ASSERT(false);
            return nullptr;
    }
}

}  // namespace Namica