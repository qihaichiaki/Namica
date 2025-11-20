#pragma once

#include <namica/core/Memory.h>
#include <namica/renderer/Framebuffer.h>
#include <namica/renderer/EditorCamera.h>

namespace Namica
{

struct EditorContext
{
    EditorCamera editorCamera;
    Ref<Framebuffer> framebuffer{nullptr};
};

}  // namespace Namica