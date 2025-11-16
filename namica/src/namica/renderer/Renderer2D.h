#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include <glm/glm.hpp>

namespace Namica
{
class NAMICA_API Renderer2D
{
public:
    /**
     * @brief 开始2D渲染场景, 此处需要上传相机的pv矩阵
     *
     * @param _pv 投影矩阵 * 视图矩阵
     */
    static void beginScene(glm::mat4 const& _pv);

private:
    static void init();

    CLASS_DISABLE_COPY_MOVE(Renderer2D)
    friend class Renderer;
};

}  // namespace Namica