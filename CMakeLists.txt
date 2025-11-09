cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(NamicaProject)

# 使用C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 编译器设置
if(WIN32 AND MSVC)
    # remove predefined warning level, suppress warning D9025
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # CXX + debug
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:/MDd>") # C/C++基础函数库, 动态库形式
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:/RTC1>") # 检查访问超出栈变量范围的内存 + 未初始化的局部变量
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>:/bigobj>") # MSVC使用扩展版COFF格式, 让每个.obj容纳更多的section(适用于大量模板元项目)

    # CXX + Release
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/MD>")
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/O2>") # 启用一系列优化选项, 让程序运行更快
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/Oi>") # 将一些常用标准库的函数优化为内联函数替换, 减少调用开销
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/Ob2>") # 运行编译器在同一模块自动内联, 包含一些小函数
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/GL>") # 全程序优化, 允许编译器在链接阶段统一优化, 比如跨模块内联
    add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:/Gy>") # 未调用的函数可以被连接器剔除, 减少生成产物的大小

    # CXX
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zi>") # 生成完整的调试信息, 存储在.pdb文件中 独立存储调试信息
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/GS>") # 栈保护, 防止修改
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/source-charset:utf-8>") # 编译器字符集使用utf-8
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/execution-charset:utf-8>") # 运行时字面量使用utf-8
    add_compile_options(/MP) # 开启并行编译
    add_compile_options(/W4) # 严格代码检查

    # add_compile_options(/WX) # 将所有警告视为错误 -> 不可全局设置, 否则一些三方库编译不通过
    add_compile_options(/permissive-) # 禁止msvc的宽松模式, 方便代码更好的平台兼容性

    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG") # Release配置下的共享库链接器，添加/DEBUG 选项

    add_compile_options(/wd4100) # 全局禁用函数参数未使用报警告
    add_compile_options(/wd4996) # 禁用msvc认为“不安全函数或弃用函数”警告
else()
    # 其他平台后续兼容添加......
endif()

add_subdirectory(namica)
add_subdirectory(namica_editor)

# 设置vs启动项目
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NamicaEditor)
endif()