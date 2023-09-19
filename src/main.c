#include "terminal.h"
#include <SDL2/SDL.h>
#include <framebuffer.h>
#include <terminal.h>
int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("terminal: not enough arguments\n");
        return -1;
    }

    framebuffer fb;
    framebuffer_new(&fb, 1024, 768, 32, (1024 * 4));

    FILE* font_fp = fopen(argv[1], "rb");
    fseek(font_fp, 0, SEEK_END);
    long size = ftell(font_fp);
    fseek(font_fp, 0, SEEK_SET);
    uint8_t* in_font = malloc(size);
    fread(in_font, size, 1, font_fp);
    fclose(font_fp);

    terminal term;
    terminal_new(&term, fb, in_font, 16, 8);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("Framebuffer Test", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, 1024, 768, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture* texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32,
                          SDL_TEXTUREACCESS_STREAMING, fb.width, fb.height);

    terminal_write_str(&term, "Hello!", 6);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            break;
        } else if (event.type == SDL_TEXTINPUT) {
            terminal_write_char(&term, event.text.text[0]);
            terminal_draw_cursor(&term, term.fg, term.bg, term.row, term.col);
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                terminal_write_char(&term, '\n');
                break;
            case SDLK_BACKSPACE:
                terminal_write_str(&term, " \b\b \b ", 3);
                terminal_draw_cursor(&term, term.fg, term.bg, term.row, term.col);
                break;
            }
        }

        // Copy the framebuffer to the texture
        void* texture_pixels;
        int texture_pitch;
        SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch);
        memcpy(texture_pixels, fb.address, fb.size);
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    free(in_font);
    return 0;
}
