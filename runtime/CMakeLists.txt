# CONFIGURE_DEPENDS: 发现新增/重命名/删除文件自动运行cmake
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${NAMICA_RUNTIME} SHARED ${SOURCES})

# 使用了C++20标准,后续依赖此必须是C++20以上
target_compile_features(${NAMICA_RUNTIME} PUBLIC cxx_std_20)

target_include_directories(${NAMICA_RUNTIME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(${NAMICA_RUNTIME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


target_link_libraries(${NAMICA_RUNTIME} PRIVATE
    stb_image
    nlohmann_json
)


target_compile_definitions(${NAMICA_RUNTIME} PRIVATE
    -DNAMICA_BUILD_DLL # 生成动态库导出信息
    # -DNAMICA_ENABLE_ASSERTS # 启用宏断点检查
    # $<$<CONFIG:DEBUG>:-DNAMICA_DEBUG> # 启动Debug模式
)

enable_project_options(${NAMICA_RUNTIME})