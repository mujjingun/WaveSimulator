project(WaveSimulator)
cmake_minimum_required(VERSION 2.8)

# Point to our own cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add global definitions
add_definitions("-Wall")
include_directories(${SDL2_INCLUDE_DIR})

add_executable(wavesimulator main.c)
install(TARGETS wavesimulator DESTINATION bin)
target_link_libraries(wavesimulator ${SDL2_LIBRARIES})