project(WindowRender)

# 自由测试: glfw
include(${CMAKE_SOURCE_DIR}/vendor/cmake/glfw.cmake)
# glad opengl现代函数加载器
include(${CMAKE_SOURCE_DIR}/vendor/cmake/glad.cmake)
# json解析库
include(${CMAKE_SOURCE_DIR}/vendor/cmake/json.cmake)

add_test_exes_from_subdirs(ON glfw glad NamicaRuntime json)

target_compile_definitions(test_window_render PRIVATE
    "NAMICA_ASSETS_DIR=\"${ASSETS_DIR}\""
)
