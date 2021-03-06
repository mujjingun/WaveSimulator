project(WaveSimulator)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Point to our own cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Find SDL2
find_package(SDL2 REQUIRED)
# Find OpenGL
find_package(OpenGL REQUIRED)
# Find GLEW
find_package(GLEW REQUIRED)

# Add global definitions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -O2")

add_executable(wavesimulator main.cpp renderer.cpp)
install(TARGETS wavesimulator DESTINATION bin)

target_include_directories(wavesimulator PRIVATE ${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(wavesimulator PRIVATE ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
