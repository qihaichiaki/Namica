include_guard(GLOBAL)

add_library(stb_image STATIC "${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image.cpp")

target_include_directories(stb_image INTERFACE
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
)