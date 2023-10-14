#version 330 core

out vec4 FragColor;

uniform vec2 window_size;
uniform vec2 center;
uniform float scale;
uniform int iterations;

void main() {
  const float LIMIT = 1000.0;
  float min_dim = min(window_size.x, window_size.y);
  vec2 xy = 2.0 * gl_FragCoord.xy - window_size;
  vec2 c = (xy / min_dim + center) / scale;
  vec2 z = vec2(0);
  int i;
  for (i = 0; i < iterations; ++i) {
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
    if (dot(z, z) > LIMIT) {
      break;
    }
  }
  float r = 1.0 - float(i) / iterations;
  FragColor = vec4(vec3(r), 1.0);
}
