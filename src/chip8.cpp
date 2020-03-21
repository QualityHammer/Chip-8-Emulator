#include <stdio.h>
#include <iostream>
#include <time.h>
#include <bitset>
#include "chip8.h"

void chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp  = 0;

    srand(time(NULL));

    for (int i{0}; i < 80; ++i) {
        memory[i] = fontset[i];
    }
}

void debugPrint(const char* msg);

void chip8::emulateCycle() {
    opcode = memory[pc] << 8 | memory[pc + 1];
std::cout << "Next Opcode: " << std::hex << opcode << std::endl;

    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x00FF) {
                // Clears the screen
                case 0x00E0:
                    for (int i{0}; i < 2048; ++i)
                        gfx[i] = 0;
                    pc += 2;
                    debugPrint("Cleared the screen.");
                break;
                // Returns from a subroutine
                case 0x00EE:
                    pc = cstack[sp];
                    --sp;
                    debugPrint("Returned from a subroutine.");
                break;
                default:
                    _unknown();
            }
        break;
        // Jumps to address at NNN
        case 0x1000:
            pc = opcode & 0x0FFF;
            debugPrint("Jumped to address NNN.");
        break;
        // Calls subroutine at NNN
        case 0x2000:
            ++sp;
            cstack[sp] = pc + 2;
            pc = opcode & 0x0FFF;
            debugPrint("Called subroutine NNN.");
        break;
        // Skips the next instruction if V[X] equals NN
        case 0x3000:
            if (V[xIndex()] == (opcode & 0x00FF)) {
                pc += 4;
                debugPrint("Skipped an instruction. V[X] equals NN.");
            }
            pc += 2;
        break;
        // Skips the next instruction if V[X] doesn't equal NN
        case 0x4000:
            if (V[xIndex()] != (opcode & 0x00FF)) {
                pc += 4;
                debugPrint("Skipped an instruction. V[X] doesn't equal NN.");
            }
            pc += 2;
        break;
        // Skips the next instruction if V[X] equals V[Y]
        case 0x5000:
            if (V[xIndex()] == V[yIndex()]) {
                pc += 4;
                debugPrint("Skipped an instruction. V[X] equals V[Y].");
            }
            pc += 2;
        break;
        // Sets V[X] to NN
        case 0x6000:
            V[xIndex()] = opcode & 0x00FF;
            pc += 2;
            debugPrint("Set V[X] to NN.");
        break;
        // Adds NN to V[X]
        case 0x7000:
            V[xIndex()] += opcode & 0x00FF;
            pc += 2;
            debugPrint("Added NN to V[X].");
        break;
        case 0x8000:
            switch(opcode & 0x000F) {
                // Sets V[X] to V[Y]
                case 0x0000:
                    V[xIndex()] = V[yIndex()];
                    pc += 2;
                    debugPrint("Set V[X] to V[Y].");
                break;
                // Sets V[X] to V[X] or V[Y]
                case 0x0001:
                    V[xIndex()] |= V[yIndex()];
                    pc += 2;
                    debugPrint("Set V[X] to V[X] | V[Y].");
                break;
                // Sets V[X] to V[X] and V[Y]
                case 0x0002:
                    V[xIndex()] &= V[yIndex()];
                    pc += 2;
                    debugPrint("Set V[X] to V[X] & V[Y].");
                break;
                // Sets V[X] to V[X] xor V[Y]
                case 0x0003:
                    V[xIndex()] ^= V[yIndex()];
                    pc += 2;
                    debugPrint("Set V[X] to V[X] ^ V[Y].");
                break;
                // Adds V[Y] to V[X] and sets carry flag if overflow
                case 0x0004:
                    if (V[yIndex()] > (0xFF - V[xIndex()]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[xIndex()] += V[yIndex()];
                    pc += 2;
                    debugPrint("Added V[X] to V[Y].");
                break;
                // Subtracts V[Y] from V[X] and sets carry flag if overflow
                case 0x0005:
                    if (V[xIndex()] > V[yIndex()])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[xIndex()] -= V[yIndex()];
                    pc += 2;
                    debugPrint("Subtracted V[Y] from V[X].");
                break;
                // Shifts V[X] to the right by 1 bit and sets carry flag if overflow
                case 0x0006:
                    if ((V[xIndex()] & 0x01) == 0x01)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[xIndex()] >>= 1;
                    pc += 2;
                    debugPrint("Shifted V[X] to the right by one bit.");
                break;
                // Sets V[X] to V[Y] minus V[X] and sets carry flag if overflow
                case 0x0007:
                    if (V[xIndex()] < V[yIndex()])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[xIndex()] = V[yIndex()] - V[xIndex()];
                    pc += 2;
                    debugPrint("Set V[X] to V[Y] minus V[X].");
                break;
                // Shifts V[X] to the left by 1 bit and sets carry flag if overflow
                case 0x000E:
                    if ((V[xIndex()] & 0x80) == 0x80)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[xIndex()] <<= 1;
                    pc += 2;
                    debugPrint("Shifted V[X] to the left by one bit.");
                break;
                default:
                    _unknown();
            }
        break;
        // Skips the next instruction if V[X] doesn't equal V[Y]
        case 0x9000:
            if (V[xIndex()] != V[yIndex()]) {
                pc += 4;
                debugPrint("Skipped an instruction. V[X] doesn't equal V[Y].");
            }
            pc += 2;
        break;
        // Sets I to NNN
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            debugPrint("Set I to NNN.");
        break;
        // Jumps to NNN plus V[0]
        case 0xB000:
            pc = V[0] + (opcode & 0x0FFF);
            debugPrint("Jumped to NNN + V[0].");
        break;
        // Sets V[X] to NN and a random number (0-255)
        case 0xC000:
            V[xIndex()] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            debugPrint("Set V[X] to NN & [a random number 0-255]");
        break;
        // Draws pixels to the graphics memory
        case 0xD000: {
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int j{0}; j < height; ++j) {
                pixel = memory[I + j];
                for (int i{0}; i < 8; ++i) {
                    if ((pixel & (0x80 >> i)) != 0) {
                        if (gfx[(V[xIndex()] + i + ((V[yIndex()] + j) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[V[xIndex()] + i + ((V[yIndex()] + j) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            debugPrint("Drew pixels to graphics memory.");
        }
        break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                // Skips the next instruction if the key stored in V[X] is pressed
                case 0x009E:
                    if (key[V[xIndex()]] != 0) {
                        pc += 4;
                        debugPrint("Skipped an instruction. The key stored in V[X] is pressed.");
                    }
                    pc += 2;
                break;
                // Skips the next instruction if the key stored in V[X] is not pressed
                case 0x00A1:
                    if (key[V[xIndex()]] != 1) {
                        pc += 4;
                        debugPrint("Skipped an instruction. The key stored in V[X] is not pressed");
                    }
                    pc += 2;
                break;
                default:
                    _unknown();
            }
        break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                // Sets V[X] to the delay timer
                case 0x0007:
                    V[xIndex()] = delayTimer;
                    pc += 2;
                    debugPrint("Set V[X] to the delay timer.");
                break;
                // All instructions are halted until a key is pressed.
                // This key is stored in V[X]
                case 0x000A:
                {
                    bool keypressed{false};
                    while (!keypressed) {
                        while (SDL_PollEvent(&event)) {
                            switch(event.type) {
                                case SDL_KEYDOWN:
                                    switch(event.key.keysym.sym) {
                                        case SDLK_1:
                                            V[xIndex()] = 1;
                                            keypressed = true;
                                        break;
                                        case SDLK_2:
                                            V[xIndex()] = 2;
                                            keypressed = true;
                                        break;
                                        case SDLK_3:
                                            V[xIndex()] = 3;
                                            keypressed = true;
                                        break;
                                        case SDLK_4:
                                            V[xIndex()] = 0xC;
                                            keypressed = true;
                                        break;
                                        case SDLK_q:
                                            V[xIndex()] = 4;
                                            keypressed = true;
                                        break;
                                        case SDLK_w:
                                            V[xIndex()] = 5;
                                            keypressed = true;
                                        break;
                                        case SDLK_e:
                                            V[xIndex()] = 6;
                                            keypressed = true;
                                        break;
                                        case SDLK_r:
                                            V[xIndex()] = 0xD;
                                            keypressed = true;
                                        break;
                                        case SDLK_a:
                                            V[xIndex()] = 7;
                                            keypressed = true;
                                        break;
                                        case SDLK_s:
                                            V[xIndex()] = 8;
                                            keypressed = true;
                                        break;
                                        case SDLK_d:
                                            V[xIndex()] = 9;
                                            keypressed = true;
                                        break;
                                        case SDLK_f:
                                            V[xIndex()] = 0xE;
                                            keypressed = true;
                                        break;
                                        case SDLK_z:
                                            V[xIndex()] = 0xA;
                                            keypressed = true;
                                        break;
                                        case SDLK_x:
                                            V[xIndex()] = 0;
                                            keypressed = true;
                                        break;
                                        case SDLK_c:
                                            V[xIndex()] = 0xB;
                                            keypressed = true;
                                        break;
                                        case SDLK_v:
                                            V[xIndex()] = 0xF;
                                            keypressed = true;
                                        break;
                                    }
                                break;
                            }
                        }
                    }
                    pc += 2;
                    debugPrint("Waited for keypress.");
                }
                break;
                // Sets the delay timer to V[X]
                case 0x0015:
                    delayTimer = V[xIndex()];
                    pc += 2;
                    debugPrint("Set the delay timer to V[X].");
                break;
                // Sets the sound timer to V[X]
                case 0x0018:
                    soundTimer = V[xIndex()];
                    pc += 2;
                    debugPrint("Set the sound timer to V[X].");
                break;
                // Adds V[X] to I and sets carry flag if overflow
                case 0x001E:
                    if (V[xIndex()] > (0xFFFF - I))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[xIndex()];
                    pc += 2;
                    debugPrint("Added V[X] to I.");
                break;
                // Sets I to the location of the srite for the character in V[X].
                // Characters 0-F are represented by a 4x5 font.
                case 0x0029:
                    I = memory[V[xIndex()]];
                    pc += 2;
                    debugPrint("Set I to the location of a sprite in V[X].");
                break;
                // Stores the binary coded decimal representation of V[X] into memory.
                // The hundreds digit is stored in location I.
                // The tens digit is stored in location I + 1.
                // The ones digit is stored in location I + 2.
                case 0x0033:
                    memory[I] = V[xIndex()] / 100;
                    memory[I + 1] = (V[xIndex()] / 10) % 10;
                    memory[I + 2] = (V[xIndex()] % 100) % 10;
                    pc += 2;
                    debugPrint("Stored a BCD representing V[X] into memory.");
                break;
                // Stores V[0]-V[X](including V[X]) in memory starting at location I.
                // The offset from I is increased by one for each value.
                // I is left unmodified.
                case 0x0055:
                    for (int i{0}; i <= (opcode & 0x0F00) >> 8; ++i)
                        memory[I + i] = V[i];
                    pc += 2;
                    debugPrint("Stored V[0]-V[X] into memory starting at location I.");
                break;
                // Fills V[0] to V[X](including V[X]) from memory starting at location I.
                // The offset from I is increased by one for each value.
                // I is left unmodified.
                case 0x0065:
                    for (int i{0}; i <= (opcode & 0x0F00) >> 8; ++i)
                        V[i] = memory[I + i];
                    pc += 2;
                    debugPrint("Filled V[0]-V[X] from memory starting at location I.");
                break;
                default:
                    _unknown();
            }
        break;
        default:
            _unknown();
    }

    if (delayTimer > 0) {
        if (delayTimer == 1)
            std::cout << "Delay finished!" << std::endl;
        --delayTimer;
    }

    if (soundTimer > 0) {
        if (soundTimer == 1)
            std::cout << "Beep!" << std::endl;
        --soundTimer;
    }
}

void chip8::loadGame(char* gamePath) {
    FILE *gameBinary{fopen(gamePath, "rb")};
    if (gameBinary == NULL) {fputs("File error.\n", stderr); exit(1);}

    fseek(gameBinary, 0, SEEK_END);
    long binarySize{ftell(gameBinary)};
    rewind(gameBinary);
    unsigned char *buffer{(unsigned char*) malloc(sizeof(unsigned char) * binarySize)};
    if (buffer == NULL) {fputs("Memory error.\n", stderr); exit(2);}

    size_t result{fread(buffer, 1, binarySize, gameBinary)};
    if (result != binarySize) {fputs("Reading error.\n", stderr); exit(3);}
    fclose(gameBinary);

    for (int i{0}; i < binarySize; ++i)
        memory[i + 512] = buffer[i];
    free(buffer);
}

void chip8::setKeys() {
    for (int i{0}; i < 16; ++i)
        key[i] = 0;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                exit(0);
            break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_1:
                        key[1] = 1;
                    break;
                    case SDLK_2:
                        key[2] = 1;
                    break;
                    case SDLK_3:
                        key[3] = 1;
                    break;
                    case SDLK_4:
                        key[0xC] = 1;
                    break;
                    case SDLK_q:
                        key[4] = 1;
                    break;
                    case SDLK_w:
                        key[5] = 1;
                    break;
                    case SDLK_e:
                        key[6] = 1;
                    break;
                    case SDLK_r:
                        key[0xD] = 1;
                    break;
                    case SDLK_a:
                        key[7] = 1;
                    break;
                    case SDLK_s:
                        key[8] = 1;
                    break;
                    case SDLK_d:
                        key[9] = 1;
                    break;
                    case SDLK_f:
                        key[0xE] = 1;
                    break;
                    case SDLK_z:
                        key[0xA] = 1;
                    break;
                    case SDLK_x:
                        key[0] = 1;
                    break;
                    case SDLK_c:
                        key[0xB] = 1;
                    break;
                    case SDLK_v:
                        key[0xF] = 1;
                    break;
                }
            break;
        }
    }
}

const unsigned char chip8::fontset[] = {
    0xF0,0x90,0x90,0x90,0xF0,0x20,0x60,0x20,0x20,0x70,0xF0,0x10,0xF0,0x80,0xF0,
    0xF0,0x10,0xF0,0x10,0xF0,0x90,0x90,0xF0,0x10,0x10,0xF0,0x80,0xF0,0x10,0xF0,
    0xF0,0x80,0xF0,0x90,0xF0,0xF0,0x10,0x20,0x40,0x40,0xF0,0x90,0xF0,0x90,0xF0,
    0xF0,0x90,0xF0,0x10,0xF0,0xF0,0x90,0xF0,0x90,0x90,0xE0,0x90,0xE0,0x90,0xE0,
    0xF0,0x80,0x80,0x80,0xF0,0xE0,0x90,0x90,0x90,0xE0,0xF0,0x80,0xF0,0x80,0xF0,
    0xF0,0x80,0xF0,0x80,0x80
};

int chip8::xIndex() {
    return (opcode & 0x0F00) >> 8;
}

int chip8::yIndex() {
    return (opcode & 0x00F0) >> 4;
}

void chip8::debugPrint(const char* msg) {
    if (debugMode)
        std::cout << "Opcode: " << std::hex << opcode << "  " << msg << std::endl;
}

void chip8::_unknown() {
    std::cout << "Unknown opcode: 0x" << opcode << std::endl;
}

