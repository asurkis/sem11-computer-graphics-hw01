cmake_minimum_required(VERSION 3.11)
project(ComputerGraphics_hw01)

find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

set(CXX_SOURCES
    src/cpp/main.cpp
    src/cpp/raii.hpp
    src/cpp/gl.hpp

    src/cpp/imgui/imgui_impl_sdl2.cpp
    src/cpp/imgui/imstb_truetype.h
    src/cpp/imgui/imgui_internal.h
    src/cpp/imgui/imstb_rectpack.h
    src/cpp/imgui/imgui_demo.cpp
    src/cpp/imgui/imstb_textedit.h
    src/cpp/imgui/imgui_impl_opengl3_loader.h
    src/cpp/imgui/imconfig.h
    src/cpp/imgui/imgui_impl_opengl3.cpp
    src/cpp/imgui/imgui_widgets.cpp
    src/cpp/imgui/imgui.cpp
    src/cpp/imgui/imgui_impl_sdl2.h
    src/cpp/imgui/imgui.h
    src/cpp/imgui/imgui_impl_opengl3.h
    src/cpp/imgui/imgui_tables.cpp
    src/cpp/imgui/imgui_draw.cpp
)
set(SHADER_SOURCES src/glsl/shader.vert src/glsl/shader.frag)
set_source_files_properties(src/glsl/shader.vert PROPERTIES SHADER_TYPE VERT)
set_source_files_properties(src/glsl/shader.frag PROPERTIES SHADER_TYPE FRAG)

add_custom_target(Shaders SOURCES ${SHADER_SOURCES}
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/gen_hexdumps.cmake"
WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)

include_directories(${SDL2_INCLUDE_DIRS} src/cpp/imgui)

add_executable(${PROJECT_NAME} ${CXX_SOURCES})
add_dependencies(${PROJECT_NAME} Shaders)

if(DEFINED EMSCRIPTEN)
    add_compile_definitions("__EMSCRIPTEN__")
    target_link_options(${PROJECT_NAME} PRIVATE "-sUSE_SDL=2")
else()
    target_link_libraries(${PROJECT_NAME} SDL2 OpenGL GLEW)
endif()

