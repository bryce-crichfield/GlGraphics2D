# FILEPATH: /home/bryce/Desktop/Electron/CMakeLists.txt
cmake_minimum_required(VERSION 3.12)
project(three)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 17)
# Use clang++ as the compiler
set(CMAKE_CXX_COMPILER g++)

# Add include directories
include_directories(Include)

# Add source files
file(GLOB SOURCES Source/*.cc Source/*.cpp)

# Add executable
add_executable(exe ${SOURCES})

# Link libraries
target_link_libraries(exe stdc++)

find_package(OpenGL REQUIRED)
target_link_libraries(exe OpenGL::GL)

find_package(glfw3 REQUIRED)
target_link_libraries(exe glfw)

find_package(GLEW REQUIRED)
target_link_libraries(exe GLEW::GLEW)

find_package(assimp REQUIRED)
target_link_libraries(exe assimp::assimp)