#include <SDL2/SDL.h>
#include <iostream>
#include <bitset>
#include <cmath>
#include <cstring>
#include "chip8.h"

chip8 emulator{true};
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
const int scale{10};
const int gWidth{64};
const int gHeight{32};
const int gPixelCount{gWidth * gHeight};
const int screenWidth{gWidth * scale};
const int screenHeight{gHeight * scale};
const int screenPixelCount{screenWidth * screenHeight};

void printError(const char* msg);

bool initSDL() {
    using namespace std;
    bool success{true};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printError("SDL could not initialize!");
        success = false;
    } else {
        gWindow = SDL_CreateWindow("Chip 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printError("Window could not be created!");
            success = false;
        } else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

void closeSDL() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

void drawGraphics() {
    Uint32 color;
    for (int i{0}; i < gPixelCount; ++i) {
        if (emulator.gfx[i] == 1)
            color = SDL_MapRGB(gScreenSurface->format, 0xFF, 0xFF, 0xFF);
        else
            color = SDL_MapRGB(gScreenSurface->format, 0, 0, 0);
        SDL_Rect rect{i % gWidth * scale, i / gWidth * scale, scale, scale};
        SDL_FillRect(gScreenSurface, &rect, color);
    }

    SDL_UpdateWindowSurface(gWindow);
}

int main(int argc, char* argv[]) {
    if (!initSDL())
        return 1;

    emulator.initialize();
    if (argc == 1) {
        char* gpath{"c8games/pong"};
        emulator.loadGame(gpath);
    } else {
        emulator.loadGame(argv[1]);
    }

    for (;;) {
        emulator.emulateCycle();

        if (emulator.drawFlag)
            drawGraphics();

        emulator.setKeys();

        SDL_Delay(70);
    }

    closeSDL();
    
    return 0;
}

void printError(const char* msg) {
    using namespace std;
    cout << msg << " SDL Error: " << SDL_GetError() << endl;
}

