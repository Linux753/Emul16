#ifndef EMUL_H
#define EMUL_H

#define MEMORY_SIZE 65536
#define CURSOR0 0x0000
#define OPCODE_NB 80
//#define STACK_SIZE 16

#define PIXEL_DIM 4
#define PIXEL_BY_WIDTH 320
#define PIXEL_BY_HEIGHT 240
#define SCREEN_WIDTH PIXEL_BY_WIDTH*PIXEL_DIM
#define SCREEN_HEIGHT PIXEL_BY_HEIGHT*PIXEL_DIM

#define PALETTE_SIZE 16

#define SPF 16 //Ms by frame
#define FREQUENCY 1000 //Expressed in ms^-1

#define CONTROL1_ADD 0xFFF0
#define CONTROL2_ADD 0xFFF2
#define STACK_INIT_ADD 0xFDF0

#define INPUT_NB 16

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>

struct SJump{
    uint32_t mask;
    uint8_t id[OPCODE_NB];
};

struct cpu16{
    uint8_t mem[MEMORY_SIZE]; //Memory size to adjust !!!
    uint16_t pc;
    int16_t reg[16];;

    uint16_t sp;

    uint8_t C;
    uint8_t Z;
    uint8_t O;
    uint8_t N;
};

struct ScreenRegister{
    
    uint8_t spritew; 
    uint8_t spriteh;
    
    bool hflip;
    bool vflip;

    bool vblank;
};

struct Screen{
    SDL_Window * window;
    SDL_Renderer * renderer;
    

    uint32_t pixelHeight;
    uint32_t pixelWidth;

    uint32_t nextRefresh;
    
    uint8_t bg;
    uint8_t fg[PIXEL_BY_WIDTH][PIXEL_BY_HEIGHT];

    struct ScreenRegister reg;

    SDL_Color palette[PALETTE_SIZE];
};

struct Control{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_bool resizeWindow;

    SDL_Scancode keyTable[INPUT_NB];

    uint16_t * control1;
    uint16_t * control2;
};

struct Sound{
    PaStream * stream;
    const PaStreamInfo * streamInfo;
    
    bool advSound;

    unsigned int attackTable[16];
    PaTime attackEndTime; 
    unsigned long attackEndI;
    float slopeAttack;

    unsigned int decayTable[16];
    PaTime decayEndTime;
    unsigned long decayEndI;
    float slopeDecay;

    unsigned int releaseTable[16];
    PaTime releaseBegTime;
    unsigned long releaseBegI;
    float slopeRelease;

    unsigned long int i; 
    unsigned long endI;

    int16_t * wave;
    int phase;
    int phaseMax; //Size of the wave array

    float volume;
    float sustain;
    uint8_t type;
};

struct Chip16{
    struct cpu16 cpu;
    struct Screen screen;
    struct Control control;
    struct Sound sound;

    struct SJump jumpTable;
    void (*opcodeFcts[OPCODE_NB])(struct Chip16 *, uint8_t, uint8_t, uint8_t);
};

void emulateChip16(struct Chip16 * chip16);

#endif

