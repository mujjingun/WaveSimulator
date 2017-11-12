#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "buffer.h"
#include "SDL.h"

class Renderer {

    // Vertex Array Objects
    VertexArrayObject<3> triangle_vao, osc_vao, wall_vao, drop_vao;

    // Buffers
    GLuint clear_buf;

    // Textures
    GLuint wave_tex[2];

    // Shader programs
    Program pass_program, pass2_program, render_program;

    // Uniform locations
    GLuint time_uni, omega_uni, dt_uni, dt2_uni;

    // Screen dimensions
    int scrwidth, scrheight;

    // Elapsed simulation time (in seconds)
    GLfloat time = 0;

public:
    Renderer(const int screen_width, const int screen_height);

    void render() noexcept;

    void mouse_click(SDL_MouseButtonEvent const& e) noexcept;
};

#endif /* RENDERER_H */
