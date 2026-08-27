include_guard(GLOBAL)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE
    "${CMAKE_SOURCE_DIR}/vendor/json"
)