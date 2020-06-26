cmake_minimum_required(VERSION 3.15)
project(OpenGLGame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D FREEGLUT_STATIC")

set(OPENGL_FILE F:/freeglut)
INCLUDE_DIRECTORIES(${OPENGL_FILE}/include)
link_directories("${OPENGL_FILE}/lib")

add_executable(OpenGLGame mul.h main.cpp semaphore.h GLUI/GL2DSlider.h GLUI/GL2DUtil.h)

target_link_libraries(OpenGLGame freeglut_static opengl32 winmm gdi32 glu32.lib)
