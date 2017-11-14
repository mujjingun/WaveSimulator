#include <vector>
#include <cstring>
#include <cmath>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "renderer.h"
#include "shader.h"
#include "vmath.h"

Renderer::Renderer(const int screen_width, const int screen_height) {

    scrwidth = screen_width;
    scrheight = screen_height;

    // Initalize Vertex Buffers
    quad_vao = VertexArrayObject<3>({
        { -1, -1, 0 },
        { 1, -1, 0 },
        { -1, 1, 0 },
        { 1, 1, 0 },
    });

    osc_vao = VertexArrayObject<3>({
        { -.7, 0, 1 },
        //{  .1, 0, 1 },
    });

#define MAKE_QUAD(x1, y1, x2, y2, c1, c2, c3, c4) \
    {x1,y1,c1},{x2,y1,c2},{x1,y2,c4},{x1,y2,c4},{x2,y1,c2},{x2,y2,c3}
    constexpr GLfloat l = -.205, r = -.2;
    wall_vao = VertexArrayObject<3>({
        // Double slit
        MAKE_QUAD(l, -1, r, -.2, 2, 2, 2, 2),
        MAKE_QUAD(l, -.15, r, .15, 2, 2, 2, 2),
        MAKE_QUAD(l, .2, r, 1, 2, 2, 2, 2),
    });

    constexpr GLfloat th = .15;
    bounds_vao = VertexArrayObject<3>({
        // Surrounding walls
        MAKE_QUAD(-1 + th, -1, 1 - th, -1 + th, 4, 4, 3, 3),
        MAKE_QUAD(-1 + th, 1 - th, 1 - th, 1, 3, 3, 4, 4),
        MAKE_QUAD(-1, -1 + th, -1 + th, 1 - th, 4, 3, 3, 4),
        MAKE_QUAD(1 - th, -1 + th, 1, 1 - th, 3, 4, 4, 3),

        // Corners
        MAKE_QUAD(-1, -1 + th, -1 + th, -1, 4, 3, 4, 4),
        MAKE_QUAD(1 - th, -1, 1, -1 + th, 4, 4, 4, 3),
        MAKE_QUAD(-1, 1 - th, -1 + th, 1, 4, 3, 4, 4),
        MAKE_QUAD(1, 1 - th, 1 - th, 1, 4, 3, 4, 4),
    });
#undef MAKE_QUAD

    drop_vao = VertexArrayObject<3>(6);

    // Create head pointer texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(2, wave_tex);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, wave_tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, scrwidth, scrheight, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    // Create Buffer for clearing textures
    glGenBuffers(1, &clear_buf);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_buf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, scrwidth * scrheight * 32, NULL, GL_STATIC_DRAW);

    void *data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    std::memset(data, 0x00, scrwidth * scrheight * 32);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    // Zero the images
    glBindTexture(GL_TEXTURE_2D, wave_tex[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scrwidth, scrheight, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Initialize Shaders
    render_program = load_shader({
        { GL_VERTEX_SHADER, "quad.vert" },
        { GL_FRAGMENT_SHADER, "quad.frag" }
    });

    size_uni = glGetUniformLocation(render_program.id(), "size");

    pass_program = load_shader({
        { GL_VERTEX_SHADER, "pass.vert" },
        { GL_FRAGMENT_SHADER, "pass.frag" }
    });

    GLuint dt_uni = glGetUniformLocation(pass_program.id(), "dt");

    glUseProgram(pass_program.id());
    glUniform1f(dt_uni, DELTA);

    pass2_program = load_shader({
        { GL_VERTEX_SHADER, "pass.vert" },
        { GL_FRAGMENT_SHADER, "pass2.frag" }
    });

    pass3_program = load_shader({
        { GL_VERTEX_SHADER, "pass.vert" },
        { GL_FRAGMENT_SHADER, "pass3.frag" }
    });

    time_uni = glGetUniformLocation(pass3_program.id(), "time");
    omega_uni = glGetUniformLocation(pass3_program.id(), "omega");
    dt_uni = glGetUniformLocation(pass3_program.id(), "dt");

    glUseProgram(pass3_program.id());
    glUniform1f(dt_uni, DELTA);
}

constexpr double period = 0.008, pi2 = 6.283185307;
constexpr double omega = pi2 / period;

void Renderer::render() noexcept {
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    static const float black[] = { 0.0, 0.0, 0.0, 0.0 };
    glClearBufferfv(GL_COLOR, 0, black);

    auto input_tex = wave_tex[0];
    auto output_tex = wave_tex[1];
    std::swap(wave_tex[0], wave_tex[1]);

    // Bind input & output image for read-write
    glBindImageTexture(0, input_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, output_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // Pass 1
    glUseProgram(pass_program.id());
    quad_vao.draw(GL_TRIANGLE_STRIP);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Pass 2
    glUseProgram(pass2_program.id());
    bounds_vao.draw(GL_TRIANGLES);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Pass 3
    glUseProgram(pass3_program.id());
    glUniform1f(time_uni, time);
    glUniform1f(omega_uni, omega);
    osc_vao.draw(GL_POINTS);
    wall_vao.draw(GL_TRIANGLES);
    drop_vao.draw(GL_POINTS);
    if (drop_vao.size()) drop_vao.pop_back();

    // Render to Screen
    glBindImageTexture(0, output_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUseProgram(render_program.id());
    glUniform2i(size_uni, scrwidth, scrheight);

    quad_vao.draw(GL_TRIANGLE_STRIP);
    wall_vao.draw(GL_TRIANGLES);
    bounds_vao.draw(GL_TRIANGLES);

    time += DELTA;
}

void Renderer::mouse_click(SDL_MouseButtonEvent const& e) noexcept {
    if (e.button == SDL_BUTTON_LEFT) {
        GLfloat x = GLfloat(e.x) / scrwidth * 2 - 1;
        GLfloat y = GLfloat(e.y) / scrheight * 2 - 1;
        drop_vao.push_back({x, y, 10});
    }
}
