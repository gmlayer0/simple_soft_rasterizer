#include <iostream>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include "vertex.h"

void half_space_rasterizer(const Vertex input[3], unsigned int width, unsigned int height, const char *tex,
                           unsigned short *db, unsigned int *fb);

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
    char *db = static_cast<char *>(malloc((WIDTH + 10) * HEIGHT * 2));

    int _i = 0;
    while (true) {
        int i = (_i < 0) ? (-_i) : _i;
        if (SDL_PollEvent(&windowEvent)) { // 对当前待处理事件进行轮询
            if (SDL_QUIT == windowEvent.type) { // 如果事件为推出SDL，结束循环
                std::cout << "SDL quit!!" << std::endl;
                break;
            }
        }
        memset(fb, 0x55, WIDTH * HEIGHT * 4);
        memset(db, 0x0, WIDTH * HEIGHT * 2);
        Vertex in1[3] = {{{200.f, 100.f, 1.0f, 1.f}, {0.45f, 0.45f}},
                         {{600.f, 100.f, 0.8f, 1.f}, {1.f,   0.f}},
                         {{200.f, 500.f, 0.8f, 1.f}, {0.f,   1.f}}};
        Vertex in2[3] = {{{400.f, 100.f, 0.8f,                     1.f}, {0.99f, 0.99f}},
                         {{400.f, 300.f, 1.0f - ((float) i / 2500), 1.f}, {0.99f, 0.99f}},
                         {{200.f, 300.f, 0.8f,                     1.f}, {0.99f, 0.99f}}};
        half_space_rasterizer(in1, WIDTH, HEIGHT, nullptr, reinterpret_cast<unsigned short *>(db),
                              reinterpret_cast<unsigned int *>(fb));
        half_space_rasterizer(in2, WIDTH, HEIGHT, nullptr, reinterpret_cast<unsigned short *>(db),
                              reinterpret_cast<unsigned int *>(fb));
        SDL_UpdateTexture(tex, nullptr, fb, WIDTH * 4);
        SDL_RenderCopy(render, tex, nullptr, nullptr);
        SDL_RenderPresent(render);
        _i++;
        if (_i > 500) _i = -500;
    }

    free(fb);
    SDL_DestroyWindow(window); // 销毁SDL窗体
    SDL_Quit(); // SDL退出
    return 0;
}