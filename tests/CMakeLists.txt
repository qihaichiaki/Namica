project(NamicaTests)

# 测试框架添加gtest依赖
include(${CMAKE_SOURCE_DIR}/vendor/cmake/gtest.cmake)

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}")

# 后续可能tests中会添加公共的utils目录
set(PUBLIC_UTILS "")

set(INCLUDE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# 按照当前测试中每个非utils目录编译一个exe进行测试生成
function(add_test_exes_from_subdirs build_main)
    set(depend_list ${ARGN})
    get_filename_component(CURRENT_DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    message("开始生成当前'${CURRENT_DIR_NAME}'测试目标下的测试<BUILD_MAIN: ${build_main}, DEPEND: ${depend_list}>")

    # 公用编译部分
    file(GLOB_RECURSE LOCAL_PUBLIC_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
    )
    file(GLOB LOCAL_PUBLIC_SOURCES_2
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )
    list(APPEND LOCAL_PUBLIC_SOURCES ${LOCAL_PUBLIC_SOURCES_2})

    # 按照测试目录中每个非utils目录(测试模块)编译一个exe进行测试生成
    set(HAS_TEST_MODULE OFF)
    file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    foreach(subdir ${SUBDIRS})
        if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
            if (NOT subdir STREQUAL "utils")
                set(HAS_TEST_MODULE ON)
                file(GLOB_RECURSE SOURCES
                    "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.cpp"
                )

                add_executable(${subdir} 
                    ${LOCAL_PUBLIC_SOURCES}
                    ${SOURCES}
                )

                target_link_libraries(${subdir} PRIVATE
                    GTest::gtest
                    $<$<BOOL:${build_main}>:GTest::gtest_main>
                    ${depend_list}
                )

                target_include_directories(${subdir} PRIVATE
                    "${INCLUDE_SOURCE_DIR}"
                )

                enable_project_options(${subdir})

                set_target_properties(${subdir} PROPERTIES 
                    RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/${CURRENT_DIR_NAME}"
                )
            endif()
        endif()
    endforeach()

    # 没有测试模块就将测试目标中平铺的cpp编译为一个exe
    if (NOT HAS_TEST_MODULE)
        add_executable(${CURRENT_DIR_NAME} 
            ${LOCAL_PUBLIC_SOURCES}
        )

        target_link_libraries(${CURRENT_DIR_NAME} PRIVATE
            GTest::gtest
            $<$<BOOL:${build_main}>:GTest::gtest_main>
            ${depend_list}
        )

        target_include_directories(${CURRENT_DIR_NAME} PRIVATE
            "${INCLUDE_SOURCE_DIR}"
        )

        enable_project_options(${CURRENT_DIR_NAME})

        set_target_properties(${CURRENT_DIR_NAME} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/${CURRENT_DIR_NAME}"
        )
    endif()
endfunction()
 

option(BUILD_PLAYGROUND "Build Playground" OFF)

# 构建runtime的相关测试
add_subdirectory(runtime)

# 自由测试
if (BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()