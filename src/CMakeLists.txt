project(Namica)

file(GLOB_RECURSE SRC_LISTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# === 1. 引入三方依赖 ===
include(${THRIDPARTY_PATH}/ThirdPartyConfig.cmake)

# === 引入三方依赖 ===

# === 2. 打印编译目录 ===
# 定义一个 map，用来记录：目录 -> 文件名列表
unset(DIR_TO_FILES)

foreach(SRC ${SRC_LISTS})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${SRC}) # 获取SRC路径相对于CMAKE_SOURCE_DIR的相对路径
    get_filename_component(DIR_NAME ${REL_PATH} DIRECTORY) # 提取相对路径的目录名
    get_filename_component(FILE_NAME ${REL_PATH} NAME) # 提取相对路径的文件名

    # 构造 key 和 value（用分号分隔列表）
    if(DIR_TO_FILES_${DIR_NAME})
        set(DIR_TO_FILES_${DIR_NAME} "${DIR_TO_FILES_${DIR_NAME}} ${FILE_NAME}")
    else()
        set(DIR_TO_FILES_${DIR_NAME} "${FILE_NAME}")
    endif()
endforeach()

# 输出每个目录的文件名
message("===>参与Namica编译的源文件:")
get_cmake_property(_vars VARIABLES) # 获取当前cmake定义的所有变量名

foreach(_var ${_vars})
    if(_var MATCHES "^DIR_TO_FILES_")
        string(REPLACE "DIR_TO_FILES_" "" DIR_NAME "${_var}")
        string(REPLACE ";" " " FILES "${${_var}}")
        message("=> [${DIR_NAME}]: (${FILES})")
    endif()
endforeach()

# === 打印编译目录 ===

# === 3. 构建项目 ===
add_executable(${PROJECT_NAME} ${SRC_LISTS})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ThirdPartyLib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DEDIT_RES_PATH="${CMAKE_SOURCE_DIR}/resources/"
)

# === 构建项目 ===

# === 4. 构建launch.json ===
# 适用于windows下vscode的运行/调试
message(STATUS "当前三方动态库路径: ${THIRD_PARTY_DLL_PATH}")
set(ENV_PATH ${THIRD_PARTY_DLL_PATH})
configure_file(
    ${CMAKE_SOURCE_DIR}/.vscode/launch.json.in
    ${CMAKE_SOURCE_DIR}/.vscode/launch.json
    @ONLY
)

# === 构建launch.json ===