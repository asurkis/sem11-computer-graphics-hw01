#version 330

out vec4 FragColor;

void main() {
    FragColor = vec4(gl_FragCoord.xy / 200, 0, 1);
}
