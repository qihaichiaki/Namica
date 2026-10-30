cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Namica)

# 使用C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectOptions.cmake")


# runtime和editor目标名字
set(NAMICA_RUNTIME "Namica")
set(NAMICA_EDITOR "NamicaEditor")

# 设置资产目录
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

# 设置生成路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# EXCLUDE_FROM_ALL 内部的库如果没人依赖将不会build
add_subdirectory(vendor EXCLUDE_FROM_ALL)
add_subdirectory(runtime)
add_subdirectory(editor)

# 设置是否生成Namica相关测试
option(NAMICA_BUILD_TESTS "Build Namica tests" ON)

# namica测试项目
if(NAMICA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


# 设置vs启动项目为NAMICA_EDITOR
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NAMICA_EDITOR})
endif()
