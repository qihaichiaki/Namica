project(IMGUI_SDL_DEMO)

file(GLOB_RECURSE SRC_LISTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_LISTS})

include(${THRIDPARTY_PATH}/ThirdPartyConfig.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2_Lib
)