#include "namica/renderer/RendererContext.h"

struct GLFWwindow;
namespace Namica
{

class OpenGLContext final : public RendererContext
{
public:
    OpenGLContext(GLFWwindow* _windowHandle);

    virtual void init() override;
    virtual void* getCurrentNativeHandle() override;
    virtual void makeCurrent(void*) override;

private:
    GLFWwindow* m_windowHandle;
};

}  // namespace Namica