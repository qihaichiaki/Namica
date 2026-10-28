include_guard(GLOBAL)

add_library(cgltf STATIC "${CMAKE_SOURCE_DIR}/vendor/cgltf/cgltf.cpp")

target_include_directories(cgltf INTERFACE
    "${CMAKE_SOURCE_DIR}/vendor/cgltf"
)