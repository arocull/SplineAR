cmake_minimum_required(VERSION 3.18.0)
project(SplineAR VERSION 0.1.0)
add_executable(SplineAR src/main.cpp)

find_package(OpenCL REQUIRED)
link_libraries(SplineAR OpenCL::OpenCL)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(SplineAR OpenGL::GL OpenGL::GLU)

# set(GLEW_USE_STATIC_LIBS true)
find_path(GLEW_INCLUDE_DIR GL/glew.h)
# find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s libGLEW)
find_package(GLEW REQUIRED STATIC)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
link_libraries(SplineAR GLEW::GLEW ${GLEW_LIBRARIES})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(SplineAR glfw ${GLFW_LIBRARIES})

include_directories(/usr/include/freetype2)

# Include all source files so they can reference each other
file(GLOB GLOB_RECURSE ${CMAKE_SOURCE_DIR} "*.h" "*.cpp")
include_directories(${CMAKE_SOURCE_DIR})

# Add compiler options
add_compile_options(-g -O2 -pthread -lGL -lGLU -lGLEW -Ifreetype2)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
