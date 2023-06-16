#include <iostream>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include "vertex.h"

void half_space_rasterizer(const Vertex input[3], unsigned int width, unsigned int height, const char *tex,
                           unsigned int *fb);

const int WIDTH = 800, HEIGHT = 600; // SDL窗口的宽和高

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) { // 初始化SDL
        std::cout << "SDL could not initialized with error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
            "SoftRender Previewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI
    ); // 创建SDL窗口

    if (window == nullptr) {
        std::cout << "SDL could not create window with error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer *render = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(render, 0x50, 0x50, 0x50, 255);
    SDL_RenderClear(render);
    SDL_RenderPresent(render);
    SDL_Texture *tex = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    SDL_Event windowEvent; // SDL窗口事件

    char *fb = static_cast<char *>(malloc((WIDTH + 10) * HEIGHT * 4));

    int _i = 0;
    while (true) {
        int i = (_i < 0) ? (-_i) : _i;
        if (SDL_PollEvent(&windowEvent)) { // 对当前待处理事件进行轮询
            if (SDL_QUIT == windowEvent.type) { // 如果事件为推出SDL，结束循环
                std::cout << "SDL quit!!" << std::endl;
                break;
            }
        }
        Vertex in[3] = {{{149.f + i,     549.f - i / 4, 1.0f, 1.f}, {0.f, 0.f}},
                        {{149.f + i / 2, 0.0f + i,      0.2f, 1.f}, {0.f, 0.f}},
                        {{549.f - i,     549.f - i,     0.0f, 1.f}, {0.f, 0.f}}};
        memset(fb, 0x55, WIDTH * HEIGHT * 4);
        half_space_rasterizer(in, WIDTH, HEIGHT, nullptr, reinterpret_cast<unsigned int *>(fb));
        SDL_UpdateTexture(tex, nullptr, fb, WIDTH * 4);
        SDL_RenderCopy(render, tex, nullptr, nullptr);
        SDL_RenderPresent(render);
//        _i++;
        if (_i > 500) _i = -500;
    }

    free(fb);
    SDL_DestroyWindow(window); // 销毁SDL窗体
    SDL_Quit(); // SDL退出
    return 0;
}