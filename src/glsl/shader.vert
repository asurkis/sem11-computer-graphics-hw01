#version 330

layout (location = 0) in vec2 pos;

out vec4 gl_Position;

void main() {
    gl_Position = vec4(pos, 0, 1);
}
