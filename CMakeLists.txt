cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Namica)

# 使用C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 编译器设置
if(MSVC)
    # remove predefined warning level, suppress warning D9025
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

function(enable_project_options target_name)
    if(NOT TARGET "${target_name}")
        message(FATAL_ERROR "目标 '${target_name}' 不存在")
    endif()

    get_target_property(target_type "${target_name}" TYPE)

    if(target_type STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    message("对目标:'${target_name}<${target_type}>'设置编译选项.")

    if(MSVC)
        target_compile_options("${target_name}" PRIVATE
            # CXX + Debug
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:/RTC1> # 检查访问超出栈变量范围的内存 + 未初始化的局部变量
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:/bigobj> # MSVC使用扩展版COFF格式, 让每个.obj容纳更多的section(适用于大量模板元项目)

            # CXX + Release
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/O2> # 启用一系列优化选项, 让程序运行更快
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Oi> # 将一些常用标准库的函数优化为内联函数替换, 减少调用开销
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Ob2> # 运行编译器在同一模块自动内联, 包含一些小函数
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/GL> # 全程序优化, 允许编译器在链接阶段统一优化, 比如跨模块内联
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Gy> # 未调用的函数可以被连接器剔除, 减少生成产物的大小

            # CXX 通用
            $<$<COMPILE_LANGUAGE:CXX>:/Zi> # 生成完整的调试信息, 存储在.pdb文件中 独立存储调试信息
            $<$<COMPILE_LANGUAGE:CXX>:/GS> # 栈保护, 防止修改
            $<$<COMPILE_LANGUAGE:CXX>:/source-charset:utf-8> # 编译器字符集使用utf-8
            $<$<COMPILE_LANGUAGE:CXX>:/execution-charset:utf-8>  # 运行时字面量使用utf-8
            $<$<COMPILE_LANGUAGE:CXX>:/MP> # 开启并行编译
            $<$<COMPILE_LANGUAGE:CXX>:/W4> # 严格代码检查
            $<$<COMPILE_LANGUAGE:CXX>:/WX> # 将所有警告视为错误
            $<$<COMPILE_LANGUAGE:CXX>:/permissive-> # 禁止msvc的宽松模式, 方便代码更好的平台兼容性
            $<$<COMPILE_LANGUAGE:CXX>:/wd4100> # 全局禁用函数参数未使用报警告
            $<$<COMPILE_LANGUAGE:CXX>:/wd4996> # 禁用msvc认为“不安全函数或弃用函数”警告
        )
    else()
        # 其他平台后续兼容添加......
    endif()

    # /MDd Debug, /MD 其他配置
    # CMake 3.15+ 推荐用 MSVC_RUNTIME_LIBRARY，非 /MD /MDd
    set_property(TARGET "${target_name}" PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    # /DEBUG 是链接选项，只对 exe/shared/module 目标设置
    if(target_type STREQUAL "EXECUTABLE"
       OR target_type STREQUAL "SHARED_LIBRARY"
       OR target_type STREQUAL "MODULE_LIBRARY")
        target_link_options("${target_name}" PRIVATE
            $<$<CONFIG:Release>:/DEBUG>
        )
    endif()
endfunction()


# runtime和editor目标名字
set(NAMICA_RUNTIME "NamicaRuntime")
set(NAMICA_EDITOR "NamicaEditor")

add_subdirectory(runtime)
add_subdirectory(editor)

enable_project_options(${NAMICA_RUNTIME})
enable_project_options(${NAMICA_EDITOR})

option(NAMICA_BUILD_TESTS "Build Namica tests" ON)

# namica测试项目
if(NAMICA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


# 设置vs启动项目
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NamicaEditor)
endif()