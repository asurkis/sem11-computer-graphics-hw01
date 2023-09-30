cmake_minimum_required(VERSION 3.11)
project(ComputerGraphics_hw01)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/cpp/main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} OpenGL)
