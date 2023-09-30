cmake_minimum_required(VERSION 3.11)
project(ComputerGraphics_hw01)

add_subdirectory(third-party/SDL)
find_package(OpenGL REQUIRED)

set(CXX_SOURCES
    src/cpp/main.cpp
    src/cpp/raii.hpp
    src/cpp/gl.hpp

    third-party/imgui/imgui_impl_sdl2.cpp
    third-party/imgui/imstb_truetype.h
    third-party/imgui/imgui_internal.h
    third-party/imgui/imstb_rectpack.h
    third-party/imgui/imgui_demo.cpp
    third-party/imgui/imstb_textedit.h
    third-party/imgui/imgui_impl_opengl3_loader.h
    third-party/imgui/imconfig.h
    third-party/imgui/imgui_impl_opengl3.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/imgui.cpp
    third-party/imgui/imgui_impl_sdl2.h
    third-party/imgui/imgui.h
    third-party/imgui/imgui_impl_opengl3.h
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/imgui_draw.cpp

    third-party/glad/src/gl.c
)
set(SHADER_SOURCES src/glsl/shader.vert src/glsl/shader.frag)
set_source_files_properties(src/glsl/shader.vert PROPERTIES SHADER_TYPE VERT)
set_source_files_properties(src/glsl/shader.frag PROPERTIES SHADER_TYPE FRAG)

add_custom_target(Shaders SOURCES ${SHADER_SOURCES}
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/gen_hexdumps.cmake"
WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)

include_directories(
    third-party/SDL/include
    third-party/glad/include
    third-party/imgui
)

add_executable(${PROJECT_NAME} ${CXX_SOURCES})
add_dependencies(${PROJECT_NAME} Shaders)

target_link_libraries(${PROJECT_NAME} SDL2::SDL2-static)

