include_guard(GLOBAL)

# 强制 GLFW 构建为静态库
set(GLFW_LIBRARY_TYPE "STATIC")
# 不构建 GLFW 自带 example / test / docs
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
# 不安装 GLFW
set(GLFW_INSTALL OFF)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/vendor/glfw"
    "${CMAKE_BINARY_DIR}/vendor/glfw"
    EXCLUDE_FROM_ALL
)

target_compile_definitions(glfw INTERFACE
    -DGLFW_INCLUDE_NONE # glfw不自己包含相关opengl头文件
)