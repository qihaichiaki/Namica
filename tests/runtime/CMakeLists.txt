project(NamicaRuntimeTest)

# 设置生成路径
set(BUILD_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})

# 公用编译部分
file(GLOB_RECURSE PUBLIC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
)
list(APPEND PUBLIC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp")

# 按照runtime测试中每个非utils目录编译一个exe进行测试生成
file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
message(STATUS "当前测试目标:")
foreach(subdir ${SUBDIRS})
    if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
        if (NOT subdir STREQUAL "utils")
            message(STATUS "${subdir}")

            file(GLOB_RECURSE SOURCES
                "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.cpp"
            )

            add_executable(${subdir} 
                ${PUBLIC_SOURCES}
                ${SOURCES}
            )

            target_link_libraries(${subdir} PRIVATE
                ${NAMICA_RUNTIME}
                GTest::gtest
            )

            target_include_directories(${subdir} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}"
            )

            enable_project_options(${subdir})
        endif()
    endif()
endforeach()