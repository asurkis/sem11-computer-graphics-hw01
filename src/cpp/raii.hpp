#ifndef RAII_hpp_INCLUDED
#define RAII_hpp_INCLUDED

#include <SDL2/SDL.h>
#include <memory>
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

#endif // RAII_hpp_INCLUDED
