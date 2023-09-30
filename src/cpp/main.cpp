#include "gl.hpp"
#include "raii.hpp"
#include "shader_sources.hpp"
#include <memory>
#include <optional>
#include <stdexcept>

struct Game {
  RAII_SDL_System _system;
  PWindow window;
  PGLContext context;
  std::optional<RAII_GL> gl;
  std::optional<ShaderProgram> shader_program;

  GLuint uniform_window_size = 0;
  GLuint uniform_center = 0;
  GLuint uniform_scale = 0;

  bool is_running = true;

  Game() : _system(SDL_INIT_VIDEO) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    window = PWindow(SDL_CreateWindow(
        "Hello, world!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800,
        600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    context = PGLContext(SDL_GL_CreateContext(window.get()));
    gl.emplace();

    init_buffers();
    init_shaders();
  }

  void init_buffers() const noexcept {
    static constexpr GLfloat vertex_data[] = {
        -1.0f, -1.0f, // 0
        -1.0f, +1.0f, // 1
        +1.0f, -1.0f, // 2
        +1.0f, +1.0f, // 3
    };
    static constexpr GLushort index_data[] = {0, 1, 2, 2, 1, 3};

    GLuint vbo = gl->buf_id(BUF_ID_VERTEX);
    GLuint ibo = gl->buf_id(BUF_ID_INDEX);
    GLuint vao = gl->vao_id(VAO_ID_FULLSCREEN);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data,
                 GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat[2]),
                          nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void init_shaders() {
    shader_program.emplace();
    Shader vert_shader(GL_VERTEX_SHADER, SRC_VERT_SHADER);
    Shader frag_shader(GL_FRAGMENT_SHADER, SRC_FRAG_SHADER);
    glAttachShader(*shader_program, vert_shader);
    glAttachShader(*shader_program, frag_shader);
    glLinkProgram(*shader_program);

    GLint log_length;
    glGetProgramiv(*shader_program, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<GLchar> info_log(log_length);
    glGetProgramInfoLog(*shader_program, info_log.size(), nullptr,
                        info_log.data());
    SDL_Log("Program linking log:\n%s", info_log.data());

    uniform_window_size = glGetUniformLocation(*shader_program, "window_size");
    uniform_center = glGetUniformLocation(*shader_program, "center");
    uniform_scale = glGetUniformLocation(*shader_program, "scale");
  }

  void main_loop_iteration() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_WINDOWEVENT) {
        if (evt.window.event == SDL_WINDOWEVENT_CLOSE) {
          is_running = false;
        } else if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
        }
      }

      if (evt.type == SDL_KEYDOWN &&
          evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        is_running = false;
      }
    }

    int window_width, window_height;
    SDL_GetWindowSize(window.get(), &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(*shader_program);
    glUniform2f(uniform_window_size, window_width, window_height);
    glUniform2f(uniform_center, 0.0f, 0.0f);
    glUniform1f(uniform_scale, 1.0f);

    glBindVertexArray(gl->vao_id(VAO_ID_FULLSCREEN));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->buf_id(BUF_ID_INDEX));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window.get());
  }
};

std::optional<Game> global;

void main_loop_iteration() { global->main_loop_iteration(); }

int main() {
  global.emplace();
  while (global->is_running) {
    main_loop_iteration();
  }
  return 0;
}
