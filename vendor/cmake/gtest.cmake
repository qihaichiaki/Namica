include_guard(GLOBAL)

# gtest / gmock 构建成静态库
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# gmock关闭
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

# 不安装gtest
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Windows MSVC 下常用，避免运行时库不一致问题
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/vendor/googletest"
    "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    EXCLUDE_FROM_ALL
)