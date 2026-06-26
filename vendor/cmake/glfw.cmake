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
    "${CMAKE_BINARY_DIR}/vendor/glfw/${CMAKE_BUILD_TYPE}"
    EXCLUDE_FROM_ALL
)

target_compile_definitions(glfw INTERFACE
    -DGLFW_INCLUDE_NONE # glfw不自己包含相关opengl头文件
)

set_target_properties(glfw PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
)