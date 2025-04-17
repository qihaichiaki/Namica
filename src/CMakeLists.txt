project(Namica)

file(GLOB_RECURSE SRC_LISTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_LISTS})

include(${THRIDPARTY_PATH}/ThirdPartyConfig.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ThirdPartyLib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DEDIT_RES_PATH="${CMAKE_SOURCE_DIR}/resources/"
)