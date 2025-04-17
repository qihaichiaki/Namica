cmake_minimum_required(VERSION 3.20.0)
project(Namica)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(THRIDPARTY_PATH "${CMAKE_SOURCE_DIR}/thirdparty")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message("当前项目构建类型: " "${BUILD_CONFIG}-${BUILD_ARCH}")

add_subdirectory(src)