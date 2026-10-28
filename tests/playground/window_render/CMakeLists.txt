add_test_exes_from_subdirs(ON glfw glad ${NAMICA_RUNTIME} nlohmann_json cgltf)

target_compile_definitions(test_window_render PRIVATE
    "NAMICA_ASSETS_DIR=\"${ASSETS_DIR}\""
)
