file(READ src/glsl/shader.vert SRC_VERT HEX)
file(READ src/glsl/shader.frag SRC_FRAG HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " HEXDUMP_VERT "${SRC_VERT}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " HEXDUMP_FRAG "${SRC_FRAG}")
configure_file(src/cpp/shader_sources.hpp.in "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/shader_sources.hpp")

