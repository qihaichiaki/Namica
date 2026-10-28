# Runtime 使用的基础依赖
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/stb_image.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/json.cmake")

# 当前测试需要使用的
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/gtest.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/glfw.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/glad.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cgltf.cmake")