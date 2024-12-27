cmake_minimum_required(VERSION 3.16)
project(opengl_fun)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Add GLAD
add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        external/glad/include
)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp
        include/shader_loader.h
        src/shader_loader.cpp
        include/stb_image.h
        src/stb_image.cpp
        include/camera.h
        src/camera.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        external/glad/include
        include
)

target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        glad
)
