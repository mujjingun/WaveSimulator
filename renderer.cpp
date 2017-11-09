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
    triangle_vao = VertexArrayObject<3>({
        { -1, -1, 0 },
        { 1, -1, 0 },
        { -1, 1, 0 },
        { 1, 1, 0 },
    });

    osc_vao = VertexArrayObject<3>({
        { -.7, 0, 1 },
        //{ .1, 0, 1 },
    });

    constexpr GLfloat l = -.305, r = -.3;
    constexpr GLfloat b = -1, t = 1;
    constexpr GLfloat b1 = -.15, t1 = -.1, b2 = .1, t2 = .15;
    wall_vao = VertexArrayObject<3>({
        { l, b1, 2 },
        { l, b, 2 },
        { r, b, 2 },
        { r, b, 2 },
        { r, b1, 2 },
        { l, b1, 2 },

        { l, b2, 2 },
        { l, t1, 2 },
        { r, t1, 2 },
        { r, t1, 2 },
        { r, b2, 2 },
        { l, b2, 2 },

        { l, t, 2 },
        { l, t2, 2 },
        { r, t2, 2 },
        { r, t2, 2 },
        { r, t, 2 },
        { l, t, 2 },
    });

    // Create head pointer texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(2, wave_tex);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, wave_tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, scrwidth, scrheight, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create Buffer for clearing the head pointer texture
    glGenBuffers(1, &clear_buf);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clear_buf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, scrwidth * scrheight * 32, NULL, GL_STATIC_DRAW);

    vec<4> *data = (vec<4> *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    std::memset(data, 0x00, scrwidth * scrheight * 32);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindTexture(GL_TEXTURE_2D, wave_tex[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, scrwidth, scrheight, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Initialize Shaders
    render_program = load_shader({
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" }
    });

    pass_program = load_shader({
        { GL_VERTEX_SHADER, "pass.vert" },
        { GL_FRAGMENT_SHADER, "pass.frag" }
    });
    time_uni = glGetUniformLocation(pass_program.id(), "time");
    omega_uni = glGetUniformLocation(pass_program.id(), "omega");
}

void Renderer::render() noexcept {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    static const float black[] = { 0.0, 0.0, 0.0, 0.0 };
    glClearBufferfv(GL_COLOR, 0, black);

    auto input_tex = wave_tex[0];
    auto output_tex = wave_tex[1];
    std::swap(wave_tex[0], wave_tex[1]);

    // Bind input & output image for read-write
    glBindImageTexture(0, input_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, output_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Shader Pass
    glUseProgram(pass_program.id());
    glUniform1f(time_uni, time);
    constexpr double period = 0.01, pi2 = 6.283185307;
    constexpr double omega = pi2 / period;
    glUniform1f(omega_uni, omega);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    triangle_vao.draw(GL_TRIANGLE_STRIP);
    osc_vao.draw(GL_POINTS);
    wall_vao.draw(GL_TRIANGLES);

    // Render to Screen
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glBindImageTexture(0, output_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUseProgram(render_program.id());
    triangle_vao.draw(GL_TRIANGLE_STRIP);
    wall_vao.draw(GL_TRIANGLES);

    time += 0.0001;
}

void Renderer::mouse_click(SDL_MouseButtonEvent const& e) noexcept {
    if (e.button == SDL_BUTTON_LEFT) {
        printf("%d %d\n", e.x, e.y);
        fflush(stdout);
        // TODO: implement raindrops
    }
}
