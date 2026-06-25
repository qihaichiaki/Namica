project(WindowRender)

# 自由测试: glfw
include(${CMAKE_SOURCE_DIR}/vendor/cmake/glfw.cmake)

add_test_exes_from_subdirs(ON glfw)