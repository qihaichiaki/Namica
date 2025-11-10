#include "namica/renderer/RendererContext.h"

struct GLFWwindow;
namespace Namica
{

class OpenGLContext final : public RendererContext
{
public:
    OpenGLContext(GLFWwindow* _windowHandle);

    virtual void init() override;

private:
    GLFWwindow* m_windowHandle;
};

}  // namespace Namica