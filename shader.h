#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <utility>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "error.h"

struct ShaderInfo {
    const GLenum type;
    const std::string filename;
};

class Shader {
    GLuint id_ = 0;
public:
    Shader() noexcept = default;
    Shader(GLuint shader) noexcept : id_(shader) {}
    ~Shader() noexcept {
        glDeleteShader(id_);
    }
    Shader (const Shader &) = delete;
    Shader &operator= (const Shader &) = delete;
    Shader (Shader &&s) noexcept
        : id_(std::exchange(s.id_, 0)) {}
    Shader &operator= (Shader &&s) noexcept {
        this->~Shader();
        return *::new (static_cast<void*>(this)) Shader(std::move(s));
    }
    GLuint id() const noexcept {
        return id_;
    }
};

class Program {
    GLuint id_ = 0;
    std::vector<Shader> shaders{};
public:
    Program() noexcept = default;
    Program(GLuint program) noexcept : id_(program) {}
    ~Program() noexcept {
        glDeleteProgram(id_);
    }
    Program (const Program &) = delete;
    Program &operator= (const Program &) = delete;
    Program (Program &&s) noexcept
        : id_(std::exchange(s.id_, 0)), shaders(std::move(s.shaders)) {}
    Program &operator= (Program &&s) noexcept {
        this->~Program();
        return *::new (static_cast<void*>(this)) Program(std::move(s));
    }
    void add_shader(Shader s) noexcept {
        shaders.push_back(std::move(s));
    }
    GLuint id() const noexcept {
        return id_;
    }
};

/* Compile & Link a shader program from files */
inline Program load_shader(std::vector<ShaderInfo> info) {
    Program program(glCreateProgram());

    for (auto const &p : info) {
        Shader shader(glCreateShader(p.type));

        std::ifstream file(p.filename);
        if (!file.good()) error("Shader file cannot be read");

        // Read whole file into string
        const std::string src(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
        const char* csrc = src.c_str();

        glShaderSource(shader.id(), 1, &csrc, NULL);
        glCompileShader(shader.id());

        GLint compiled;
        glGetShaderiv(shader.id(), GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLsizei len;
            glGetShaderiv(shader.id(), GL_INFO_LOG_LENGTH, &len);

            std::vector<GLchar> log(len);
            glGetShaderInfoLog(shader.id(), len, &len, log.data());
            error(log.data());
        }

        glAttachShader(program.id(), shader.id());

        program.add_shader(std::move(shader));
    }

    glLinkProgram(program.id());

    GLint linked;
    glGetProgramiv(program.id(), GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei len;
        glGetShaderiv(program.id(), GL_INFO_LOG_LENGTH, &len);

        std::vector<GLchar> log(len);
        glGetShaderInfoLog(program.id(), len, &len, log.data());
        error(log.data());
    }

    return program;
}

#endif /* SHADER_H */
