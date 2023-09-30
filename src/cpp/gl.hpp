#ifndef gl_hpp_INCLUDED
#define gl_hpp_INCLUDED

#include <glad/gl.h>
#include <SDL.h>
#include <cstring>
#include <stdexcept>
#include <vector>

enum BufferId { BUF_ID_VERTEX = 0, BUF_ID_INDEX, BUF_TOTAL };
enum VaoId { VAO_ID_FULLSCREEN = 0, VAO_TOTAL };

struct RAII_GL {
  RAII_GL() {
    glGenBuffers(BUF_TOTAL, buf);
    glGenVertexArrays(VAO_TOTAL, vao);
  }

  ~RAII_GL() { 
      glDeleteVertexArrays(VAO_TOTAL, vao);
      glDeleteBuffers(BUF_TOTAL, buf); 
  }

  RAII_GL(const RAII_GL &) = delete;
  RAII_GL &operator=(const RAII_GL &) = delete;

  GLuint buf_id(BufferId id) const noexcept { return buf[id]; }
  GLuint vao_id(VaoId id) const noexcept { return vao[id]; }

private:
  GLuint buf[BUF_TOTAL];
  GLuint vao[VAO_TOTAL];
};

struct Shader {
  Shader(GLenum type, const char *source) : idx(glCreateShader(type)) {
    const GLchar *source_data[] = {source};
    const GLint source_length[] = {static_cast<GLint>(std::strlen(source))};
    glShaderSource(idx, 1, source_data, source_length);
    glCompileShader(idx);

    GLint log_length;
    glGetShaderiv(idx, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<GLchar> log(log_length);
    glGetShaderInfoLog(idx, log.size(), nullptr, log.data());
    SDL_Log("Compiling shader with source:\n////////////////\n%s\n////////////////", source);
    SDL_Log("Shader compilation log:\n%s", log.data());
  }

  ~Shader() { glDeleteShader(idx); }

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  GLuint get() const noexcept { return idx; }
  operator GLuint() const noexcept { return get(); }

private:
  GLuint idx;
};

struct ShaderProgram {
  ShaderProgram() : idx(glCreateProgram()) {}
  ~ShaderProgram() { glDeleteProgram(idx); }

  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  GLuint get() const noexcept { return idx; }
  operator GLuint() const noexcept { return get(); }

private:
  GLuint idx;
};

#endif // gl_hpp_INCLUDED
