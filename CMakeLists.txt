cmake_minimum_required(VERSION 3.16)
project(opengl_fun)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

if(WIN32)
    message(STATUS "Building for Windows")
    set(GLFW_INCLUDE_DIR "C:/glfw-3.4.bin.WIN64/include")
    set(GLFW_LIBRARY "C:/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a")
elseif(UNIX)
    message(STATUS "Building for Linux/UNIX")
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARY glfw)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()


# Include directories (common for both platforms)
include_directories(${GLFW_INCLUDE_DIR})


# Add GLAD
add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        external/glad/include
)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp
        include/shader.h
        src/shader.cpp
        include/stb_image.h
        src/stb_image.cpp
        include/camera.h
        src/camera.cpp
        include/particle.h
        include/compute_shader.h
        src/compute_shader.cpp
        include/particle_system.h
        src/particle_system.cpp
        include/octree_node.h
        include/octree.h
        src/octree.cpp
        include/barnes_hut.h
        src/barnes_hut.cpp
        include/buffer_manager.h
        include/timers.h
        include/thread_pool.h)

target_include_directories(${PROJECT_NAME} PRIVATE
        external/glad/include
        include
)

target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        ${GLFW_LIBRARY}
        glad
)
