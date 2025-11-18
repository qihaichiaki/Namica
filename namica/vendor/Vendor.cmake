# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

# glfw
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glfw)

target_include_directories(glfw INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/glfw/include"
)

target_compile_definitions(glfw INTERFACE
    -DGLFW_INCLUDE_NONE # glfw不自己包含相关opengl头文件
)

# yaml
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)

target_include_directories(yaml-cpp INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/include"
)

# glad
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glad)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/glm"
)

# vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "未检测到 Vulkan SDK 环境变量 VULKAN_SDK")
endif()

set(VULKAN_SDK $ENV{VULKAN_SDK})
message(STATUS "Vulkan SDK found at ${VULKAN_SDK}")
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK}/Lib")

# 所有 Vulkan SDK 的库
set(VULKAN_SDK_LIBS_DEBUG
    "${VULKAN_LIBRARY_DIR}/vulkan-1.lib"
    "${VULKAN_LIBRARY_DIR}/shaderc_sharedd.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-cored.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-glsld.lib"
    "${VULKAN_LIBRARY_DIR}/SPIRV-Toolsd.lib"
)

set(VULKAN_SDK_LIBS_RELEASE
    "${VULKAN_LIBRARY_DIR}/vulkan-1.lib"
    "${VULKAN_LIBRARY_DIR}/shaderc_shared.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-core.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-glsl.lib"
)

add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE
    "${VULKAN_INCLUDE_DIR}" # vulkan sdk相关include
)
target_link_libraries(vulkan INTERFACE
    $<$<CONFIG:Debug>:${VULKAN_SDK_LIBS_DEBUG}>
    $<$<CONFIG:Release>:${VULKAN_SDK_LIBS_RELEASE}>
)

# stb_image
add_library(stb_image STATIC "${CMAKE_CURRENT_LIST_DIR}/stb_image/stb_image.cpp")

target_include_directories(stb_image INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/stb_image"
)

# imgui
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/imgui)