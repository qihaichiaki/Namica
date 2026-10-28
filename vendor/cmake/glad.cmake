include_guard(GLOBAL)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/vendor/glad"
    "${CMAKE_BINARY_DIR}/vendor/glad"
    EXCLUDE_FROM_ALL
)