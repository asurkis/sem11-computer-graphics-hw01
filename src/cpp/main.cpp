#include "gl.hpp"
#include "raii.hpp"
#include "shader_sources.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <memory>
#include <optional>
#include <sstream>
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
  GLuint uniform_iterations = 0;

  Game() : _system(SDL_INIT_VIDEO) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    window = PWindow(SDL_CreateWindow(
        "Hello, world!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800,
        600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    context = PGLContext(SDL_GL_CreateContext(window.get()));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
    ImGui_ImplOpenGL3_Init();

    gl.emplace();

    // Disable V-Sync
    SDL_GL_SetSwapInterval(0);

    init_buffers();
    init_shaders();
  }

  ~Game() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
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
    uniform_iterations = glGetUniformLocation(*shader_program, "iterations");
  }

  Uint32 fps_update_interval = 1000;
  Uint32 fps_last_tick = 0;
  Uint32 last_frame_tick = 0;
  int frames_passed = 0;

  Uint32 last_update_tick = 0;
  Uint32 next_update_tick = 0;
  float last_center_x = 0.0f;
  float last_center_y = 0.0f;
  float next_center_x = 0.0f;
  float next_center_y = 0.0f;
  float last_scale = 1.0f;
  float next_scale = 1.0f;

  double lerp_time(double last, double next) const noexcept {
    if (last_update_tick == next_update_tick) {
      return next;
    }
    double progress = double(last_frame_tick - last_update_tick) /
                      (next_update_tick - last_update_tick);
    progress = std::max(0.0, std::min(1.0, progress));
    return last + (next - last) * progress;
  }

  double curr_center_x() const noexcept {
    return lerp_time(last_center_x, next_center_x);
  }

  double curr_center_y() const noexcept {
    return lerp_time(last_center_y, next_center_y);
  }

  double curr_scale() const noexcept {
    return lerp_time(last_scale, next_scale);
  }

  void update_time() {
    Uint32 current_tick = SDL_GetTicks();
    Uint32 ms_passed = current_tick - fps_last_tick;

    if (ms_passed > fps_update_interval) {
      float fps = 1000.0f * frames_passed / ms_passed;
      std::ostringstream oss;
      oss.setf(std::ios::fixed);
      oss.precision(1);
      oss << "FPS: " << fps;
      SDL_SetWindowTitle(window.get(), oss.str().c_str());
      fps_last_tick = current_tick;
      frames_passed = 0;
    }

    ++frames_passed;

    Uint32 dt = current_tick - last_frame_tick;
    last_frame_tick = current_tick;
  }

  Uint32 transition_ticks = 125;
  double scroll_coef = 0.25;

  void scroll(Sint32 x, Sint32 y, Sint32 delta) {
    int w, h;
    SDL_GetWindowSize(window.get(), &w, &h);
    int min_dim = std::min(w, h);

    double cx = curr_center_x();
    double cy = curr_center_y();
    double s = curr_scale();

    last_center_x = cx;
    last_center_y = cy;
    last_scale = s;
    next_scale *= 1.0 + scroll_coef * delta;
    last_update_tick = last_frame_tick;
    next_update_tick = last_update_tick + transition_ticks;

    // ssx (screen space x) = (2x - w) / min_dim
    // preserve_x = (ssx + cx) / s;
    // (ssx + cx') / s' = (ssx + cx) / s
    // cx' / s' = ssx * (1/s - 1/s') + cx / s
    // cx' = (s' / s - 1) * ssx + s' / s * cx
    double ratio = next_scale / s;
    next_center_x = (ratio - 1.0) * (2 * x - w) / min_dim + ratio * cx;
    next_center_y = (ratio - 1.0) * (h - 2 * y) / min_dim + ratio * cy;
  }

  void drag(Sint32 xrel, Sint32 yrel) {
    int w, h;
    SDL_GetWindowSize(window.get(), &w, &h);
    int min_dim = std::min(w, h);

    double cx = curr_center_x();
    double cy = curr_center_y();
    double s = curr_scale();

    // ((2x - w) / min_dim + cx) / s = ((2x' - w) / min_dim + cx') / s
    // (2x - w) / min_dim + cx = (2x' - w) / min_dim + cx'
    // dcx = -2dx / min_dim
    cx -= 2.0 * xrel / min_dim;
    cy += 2.0 * yrel / min_dim;

    last_center_x = next_center_x = cx;
    last_center_y = next_center_y = cy;
    last_scale = next_scale = s;
    last_update_tick = next_update_tick = last_frame_tick;
  }

  bool is_running = true;

  void poll_events() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      ImGui_ImplSDL2_ProcessEvent(&evt);

      if (evt.type == SDL_WINDOWEVENT &&
          evt.window.event == SDL_WINDOWEVENT_CLOSE) {
        is_running = false;
      } else if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          is_running = false;
        } else if (evt.key.keysym.scancode == SDL_SCANCODE_EQUALS) {
          double cx = curr_center_x();
          double cy = curr_center_y();
          double s = curr_scale();

          last_center_x = cx;
          last_center_y = cy;
          last_scale = s;
          next_scale = 1.0;
          next_center_x = 0.0;
          next_center_y = 0.0;
          last_update_tick = last_frame_tick;
          next_update_tick = last_update_tick + transition_ticks;
        }
      } else if (evt.type == SDL_MOUSEWHEEL) {
        scroll(evt.wheel.mouseX, evt.wheel.mouseY, evt.wheel.y);
      } else if (evt.type == SDL_MOUSEMOTION) {
        if (evt.motion.state & SDL_BUTTON_LMASK) {
          drag(evt.motion.xrel, evt.motion.yrel);
        }
      }
    }
  }

  int mandelbrot_iters = 256;

  void redraw() {
    int window_width, window_height;
    SDL_GetWindowSize(window.get(), &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(*shader_program);
    glUniform2f(uniform_window_size, window_width, window_height);
    glUniform2f(uniform_center, curr_center_x(), curr_center_y());
    glUniform1f(uniform_scale, curr_scale());
    glUniform1i(uniform_iterations, mandelbrot_iters);

    glBindVertexArray(gl->vao_id(VAO_ID_FULLSCREEN));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->buf_id(BUF_ID_INDEX));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Settings window");
    ImGui::SliderInt("Iterations", &mandelbrot_iters, 1, 1024);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window.get());
  }

  void main_loop_iteration() {
    update_time();
    poll_events();
    redraw();
  }
};

std::optional<Game> game;

void main_loop_iteration() { game->main_loop_iteration(); }

int main() {
  game.emplace();
  while (game->is_running) {
    main_loop_iteration();
  }
  return 0;
}
