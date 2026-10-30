add_test_exes_from_subdirs(OFF ${NAMICA_RUNTIME})

# 接入CTest
include(GoogleTest)

# 将当前目录创建的测试程序统一注册到 CTest
get_property(testTargets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)

foreach(testTarget IN LISTS testTargets)
    get_target_property(targetType "${testTarget}" TYPE)

    if(targetType STREQUAL "EXECUTABLE")
        gtest_discover_tests("${testTarget}")
    endif()
endforeach()