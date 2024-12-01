# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(Skybox_Project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
set(SOURCES
    src/main.cpp
    src/Skybox.cpp
    src/Camera.cpp
    src/utils.cpp
    src/ObjLoader.cpp
    src/tiny_obj_loader.cc
)

# 添加头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# 查找 OpenGL、GLEW 和 GLFW 库
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

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
)

# 处理着色器文件
file(GLOB SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/shaders/*.frag
)

# 复制着色器文件到构建目录
foreach(
    SHADER_FILE
    ${SHADER_SOURCES}
)
get_filename_component(
    SHADER_NAME
    ${SHADER_FILE}
    NAME
)
configure_file(
    ${SHADER_FILE}
    ${CMAKE_BINARY_DIR}/${SHADER_NAME}
    COPYONLY
)
endforeach()