#version 330

out vec4 FragColor;

uniform vec2 window_size;
uniform vec2 center;
uniform float scale;
uniform int iterations;

void main() {
  float min_dim = min(window_size.x, window_size.y);
  vec2 xy = 2.0 * gl_FragCoord.xy - window_size;
  vec2 c = (xy / min_dim + center) / scale;
  vec2 z = vec2(0);
  for (int i = 0; i < iterations; ++i) {
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
  }
  float r = 1.0 - exp(-length(z));
  FragColor = vec4(vec3(r), 1.0);
}
