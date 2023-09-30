#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <memory>
#include <optional>
#include <stdexcept>

struct RAII_SDL_System {
  RAII_SDL_System(Uint32 flags) {
    if (SDL_Init(flags)) {
      throw std::runtime_error("Could not initialize SDL");
    }
  };
  ~RAII_SDL_System() { SDL_Quit(); }
};

template <typename T, void (*D)(T *)> struct DeleterSDL {
  void operator()(T *val) const noexcept { D(val); }
};

template <typename T, void (*D)(T *)>
using UniquePtrSDL = std::unique_ptr<T, DeleterSDL<T, D>>;

using PWindow = UniquePtrSDL<SDL_Window, SDL_DestroyWindow>;
using PGLContext = UniquePtrSDL<void, SDL_GL_DeleteContext>;

struct RAII_SDL_Window {
  RAII_SDL_Window(const char *name, int x, int y, int w, int h, Uint32 flags)
      : mWindow(SDL_CreateWindow(name, x, y, w, h, flags)) {
    if (!mWindow) {
      throw std::runtime_error("Could not create window");
    }
  }
  ~RAII_SDL_Window() { SDL_DestroyWindow(get()); }

  SDL_Window *get() const noexcept { return mWindow; }
  operator SDL_Window *() const noexcept { return get(); };

private:
  SDL_Window *mWindow;
};

struct RAII {
  RAII_SDL_System _system;
  PWindow window;
  PGLContext context;

  RAII() : _system(SDL_INIT_VIDEO) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    window = PWindow(SDL_CreateWindow("Hello, world!", SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, 800, 600,
                                      SDL_WINDOW_OPENGL));
    context = PGLContext(SDL_GL_CreateContext(window.get()));
  }

  bool is_running = true;
};

std::optional<RAII> global;

void main_loop_iteration() {
  SDL_Event evt;
  while (SDL_PollEvent(&evt)) {
    if (evt.type == SDL_WINDOWEVENT &&
        evt.window.event == SDL_WINDOWEVENT_CLOSE) {
      global->is_running = false;
    }

    if (evt.type == SDL_KEYDOWN &&
        evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
      global->is_running = false;
    }
  }

  glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(global->window.get());
}

int main() {
  global.emplace();
  while (global->is_running) {
    main_loop_iteration();
  }
  return 0;
}
