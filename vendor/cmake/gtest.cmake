include_guard(GLOBAL)

# gtest / gmock 构建成静态库
set(BUILD_SHARED_LIBS OFF)
# gmock关闭
set(BUILD_GMOCK OFF)
# 不安装gtest
set(INSTALL_GTEST OFF)

# Windows MSVC 下常用，避免运行时库不一致问题
if(MSVC)
    set(gtest_force_shared_crt ON)
endif()

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/vendor/googletest"
    "${CMAKE_BINARY_DIR}/vendor/googletest/${CMAKE_BUILD_TYPE}"
    EXCLUDE_FROM_ALL
)

set_target_properties(gtest PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
)
set_target_properties(gtest_main PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
)