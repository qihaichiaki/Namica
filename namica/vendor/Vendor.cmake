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