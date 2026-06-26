include_guard(GLOBAL)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/vendor/glad"
    "${CMAKE_BINARY_DIR}/vendor/glad/${CMAKE_BUILD_TYPE}"
    EXCLUDE_FROM_ALL
)

set_target_properties(glfw PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/vendor/${CMAKE_BUILD_TYPE}"
)