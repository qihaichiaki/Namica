include_guard(GLOBAL)

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

add_library(cgltf STATIC "${CMAKE_SOURCE_DIR}/vendor/cgltf/cgltf.cpp")

target_include_directories(cgltf INTERFACE
    "${CMAKE_SOURCE_DIR}/vendor/cgltf"
)