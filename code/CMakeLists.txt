# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(firework)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 别打开 openmp 编译指令了 不然我 mac 上不好跑
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# 添加源文件
set(SOURCES
    src/main.cpp
    src/Sky.cpp
    src/Skybox.cpp
    src/Ground.cpp
    src/Camera.cpp
    src/utils.cpp
    src/ObjLoader.cpp
    src/tiny_obj_loader.cc
    src/Particle.cpp
    src/ParticleRenderer.cpp
    src/Fireworks.cpp
)

# 添加头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/patterns
)

# 查找 OpenGL、GLEW 和 GLFW 库
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# 检查是否找到依赖
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found!")
endif()

if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found!")
endif()

# 添加可执行文件
add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    GLEW::GLEW
    glfw
    sfml-audio
)

# 处理着色器文件
file(GLOB SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/shaders/*.frag
)

# # 复制着色器文件到构建目录
# foreach(
#     SHADER_FILE
#     ${SHADER_SOURCES}
# )
# get_filename_component(
#     SHADER_NAME
#     ${SHADER_FILE}
#     NAME
# )
# configure_file(
#     ${SHADER_FILE}
#     ${CMAKE_BINARY_DIR}/${SHADER_NAME}
#     COPYONLY
# )
# endforeach()
