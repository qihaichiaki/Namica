# 三方库相关依赖

message("当前三方库依赖所在目录:" ${CMAKE_CURRENT_LIST_DIR})
add_library(SDL2_Lib INTERFACE)

target_include_directories(SDL2_Lib INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/SDL2/include"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_image/include"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer/include"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_ttf/include"
)

target_link_directories(SDL2_Lib INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/SDL2/lib/x64"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_image/lib/x64"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer/lib/x64"
    "${CMAKE_CURRENT_LIST_DIR}/SDL2_ttf/lib/x64"
)

target_link_libraries(SDL2_Lib INTERFACE
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)