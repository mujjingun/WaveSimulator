#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "buffer.h"
#include "SDL.h"

class Renderer {

    // Vertex Array Objects
    VertexArrayObject<3> quad_vao, osc_vao, wall_vao, bounds_vao, drop_vao;

    // Buffers
    GLuint clear_buf;

    // Textures
    GLuint wave_tex[2];

    // Shader programs
    Program pass_program, pass3_program, render_program;

    // Uniform locations
    GLuint time_uni, omega_uni, size_uni;

    // Screen dimensions
    int scrwidth, scrheight;

    // Elapsed simulation time (in seconds)
    GLfloat time = 0;

public:

    // Tick length
    constexpr static double DELTA = 1. / 10000;

    // How many ticks to simulate per render frame
    constexpr static int render_period = 20;

    Renderer(const int screen_width, const int screen_height);

    void render() noexcept;

    void mouse_click(SDL_MouseButtonEvent const& e) noexcept;
};

#endif /* RENDERER_H */
