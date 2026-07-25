include_guard(GLOBAL)

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

add_library(stb_image STATIC "${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image.cpp")

target_include_directories(stb_image INTERFACE
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
)