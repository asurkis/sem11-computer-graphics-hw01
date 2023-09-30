cmake_minimum_required(VERSION 3.11)
project(ComputerGraphics_hw01)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

set(CXX_SOURCES src/cpp/main.cpp src/cpp/raii.hpp src/cpp/gl.hpp)
set(SHADER_SOURCES src/glsl/shader.vert src/glsl/shader.frag)
set_source_files_properties(src/glsl/shader.vert PROPERTIES SHADER_TYPE VERT)
set_source_files_properties(src/glsl/shader.frag PROPERTIES SHADER_TYPE FRAG)
add_custom_target(Shaders SOURCES ${SHADER_SOURCES})

# Hex dumps
file(READ src/glsl/shader.vert SRC_VERT HEX)
file(READ src/glsl/shader.frag SRC_FRAG HEX)
string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " HEXDUMP_VERT "${SRC_VERT}")
string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " HEXDUMP_FRAG "${SRC_FRAG}")
configure_file(src/cpp/shader_sources.hpp.in "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/shader_sources.hpp")

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${CXX_SOURCES})
target_link_libraries(${PROJECT_NAME} SDL2 OpenGL GLEW)
add_dependencies(${PROJECT_NAME} Shaders)
