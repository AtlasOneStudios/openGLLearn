cmake_minimum_required(VERSION 3.31)
project(openGLLearn)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(libraries/ libraries/GLFW/include/ )
LINK_DIRECTORIES(libraries/GLFW/lib/)

add_executable(openGLLearn source/main.cpp libraries/glad/glad.c
        source/SHADER.h
        source/SHADER.cpp)

target_link_libraries(openGLLearn libglfw3.a ${OPENGL_LIBRARIES})