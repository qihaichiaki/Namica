project(Namica)

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

# 将所有警告视为错误
target_compile_options(${PROJECT_NAME} PRIVATE /WX)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DNAMICA_BUILD_DLL # 生成动态库导出信息
    -DNAMICA_ENABLE_ASSERTS # 启用宏断点检查
    -DNAMICA_DEBUG # 启动Debug模式
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# 引入三方库
include(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Vendor.cmake)

# 链接三方库
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)

# 确定后期是否调整, 前期方便使用
target_link_libraries(${PROJECT_NAME} PUBLIC
    yaml-cpp
)