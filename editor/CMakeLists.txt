file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${NAMICA_EDITOR} ${SOURCES})

target_compile_definitions(${NAMICA_EDITOR} PRIVATE
    # $<$<CONFIG:DEBUG>:-DNAMICA_DEBUG> # 启动Debug模式
)

target_include_directories(${NAMICA_EDITOR} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${NAMICA_EDITOR} PRIVATE
    ${NAMICA_RUNTIME}
)

enable_project_options(${NAMICA_EDITOR})