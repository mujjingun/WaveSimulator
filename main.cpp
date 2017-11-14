#include <GL/glew.h>
#include "SDL.h"
#include "renderer.h"
#include "error.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main(int argc, char ** argv) {
    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    // Request opengl 4.4 context.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

    SDL_Window *window = SDL_CreateWindow("WaveSimulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initalize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() !=  GLEW_OK) error("Glew Cannot be Initialized.");

    // Renderer scope
    {
        Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
        int last_ms = SDL_GetTicks();
        int framecount = 0;

        // Main game loop
        for (; !SDL_QuitRequested(); ++framecount) {
            int this_ms = SDL_GetTicks();
            int delta = this_ms - last_ms;
            if (delta >= 1000) {
                double fps = double(framecount) / (delta / 1000.);
                printf("%.2lf Hz, %.0lfx slowdown\n", fps, 1 / fps / renderer.DELTA);
                fflush(stdout);
                last_ms = this_ms;
                framecount = 0;
            }

            // Handle events on queue
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                // Handle button events
                if (e.type == SDL_MOUSEBUTTONUP) {
                    e.button.y = WINDOW_HEIGHT - e.button.y;
                    renderer.mouse_click(e.button);
                }
            }

            SDL_RenderClear(sdl_renderer);

            renderer.render();

            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                error((char *)gluErrorString(err));
            }

            SDL_RenderPresent(sdl_renderer);
        }
    }

    // Deinitialize everything
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
