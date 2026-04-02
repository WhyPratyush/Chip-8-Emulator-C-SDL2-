#include<iostream>
#include<chrono>
#include<thread>
#include<SDL2/SDL.h>
#include "chip8.cpp"

constexpr int WIDTH = 64;
constexpr int HEIGHT = 32;
constexpr float INTERVAL = 16.66f;

using namespace std;

int main(int argc, char* argv[]) {
    chip8 Emu;
    Emu.initialise();

    if (argc < 2){
        cout<<"Usage: "<<argv[0]<<" <ROM file>\n";
        return 1;
    }

    if(!Emu.loadROM(argv[1])) {
        cout<<"File doesn't exist!"<<endl;
        return -1; 
    }
    int scale = 10;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout<<"SDL cannot initialize! SDL_Error: "<<SDL_GetError()<<endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIDTH * scale, HEIGHT * scale, SDL_WINDOW_SHOWN
    );

    if(!window) {
        cout<<"Window cannot be created! SDL_Error: "<<SDL_GetError()<<endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!renderer) {
        cout<<"Renderer cannot be created! SDL_Error: "<<SDL_GetError()<<endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    if(!texture) {
        cout<<"Texture cannot be created! SDL_Error: "<<SDL_GetError()<<endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    auto last = chrono::steady_clock::now();
    bool exit = false;
    SDL_Event E;

    uint32_t pixels[WIDTH * HEIGHT];

    while(!exit) {
        while (SDL_PollEvent(&E) != 0) {
            if (E.type == SDL_QUIT) exit = true;

            if (E.type == SDL_KEYDOWN) {
                switch (E.key.keysym.sym) {
                    case SDLK_1: Emu.keypad[0x1] = 1; break;
                    case SDLK_2: Emu.keypad[0x2] = 1; break;
                    case SDLK_3: Emu.keypad[0x3] = 1; break;
                    case SDLK_4: Emu.keypad[0xC] = 1; break;

                    case SDLK_q: Emu.keypad[0x4] = 1; break;
                    case SDLK_w: Emu.keypad[0x5] = 1; break;
                    case SDLK_e: Emu.keypad[0x6] = 1; break;
                    case SDLK_r: Emu.keypad[0xD] = 1; break;

                    case SDLK_a: Emu.keypad[0x7] = 1; break;
                    case SDLK_s: Emu.keypad[0x8] = 1; break;
                    case SDLK_d: Emu.keypad[0x9] = 1; break;
                    case SDLK_f: Emu.keypad[0xE] = 1; break;

                    case SDLK_z: Emu.keypad[0xA] = 1; break;
                    case SDLK_x: Emu.keypad[0x0] = 1; break;
                    case SDLK_c: Emu.keypad[0xB] = 1; break;
                    case SDLK_v: Emu.keypad[0xF] = 1; break;
                }
            }

            if (E.type == SDL_KEYUP) {
                switch (E.key.keysym.sym) {
                    case SDLK_1: Emu.keypad[0x1] = 0; break;
                    case SDLK_2: Emu.keypad[0x2] = 0; break;
                    case SDLK_3: Emu.keypad[0x3] = 0; break;
                    case SDLK_4: Emu.keypad[0xC] = 0; break;

                    case SDLK_q: Emu.keypad[0x4] = 0; break;
                    case SDLK_w: Emu.keypad[0x5] = 0; break;
                    case SDLK_e: Emu.keypad[0x6] = 0; break;
                    case SDLK_r: Emu.keypad[0xD] = 0; break;

                    case SDLK_a: Emu.keypad[0x7] = 0; break;
                    case SDLK_s: Emu.keypad[0x8] = 0; break;
                    case SDLK_d: Emu.keypad[0x9] = 0; break;
                    case SDLK_f: Emu.keypad[0xE] = 0; break;

                    case SDLK_z: Emu.keypad[0xA] = 0; break;
                    case SDLK_x: Emu.keypad[0x0] = 0; break;
                    case SDLK_c: Emu.keypad[0xB] = 0; break;
                    case SDLK_v: Emu.keypad[0xF] = 0; break;
                }
            }
        }

        Emu.cycle();
        auto cur = chrono::steady_clock::now();
        auto diff = chrono::duration<float, milli>(cur - last).count();

        while (diff >= INTERVAL) {
            Emu.updateTimers();
            last += chrono::microseconds(16666);
            diff -= INTERVAL;

            if(SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t)) != 0) {
                cout<<"Texture update failed: "<<SDL_GetError()<<endl;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            if(SDL_RenderCopy(renderer, texture, nullptr, nullptr) != 0) {
                cout<<"RenderCopy failed: "<<SDL_GetError()<<endl;
            }

            SDL_RenderPresent(renderer);
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                pixels[y * WIDTH + x] = (Emu.display[y][x] == 1) ? 0xFFFFFFFF : 0x000000FF;
            }
        }

        for(int i = 0; i < 16; i++) {
            Emu.prevKeypad[i] = Emu.keypad[i];
        }

        this_thread::sleep_for(chrono::microseconds(1200));
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}