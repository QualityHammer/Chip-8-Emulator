#pragma once
#ifndef CHIP8
#define CHIP8
#include <string>
#include <SDL2/SDL.h>

using namespace std;
class chip8 {
public:
    chip8() : debugMode{false} {}
    chip8(const bool debug) : debugMode{debug} {}

    const bool debugMode;
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char gfx[2048];
    unsigned char delayTimer;
    unsigned char soundTimer;
    unsigned short cstack[16];
    unsigned short sp;
    unsigned char key[16];
    bool drawFlag;
    static const unsigned char fontset[];
    SDL_Event event;

    void initialize();
    void emulateCycle();
    void loadGame(char* gamePath);
    void setKeys();

    int xIndex();
    int yIndex();
    void debugPrint(const char* msg);
    void _unknown();
};

#endif
