#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "emul.h"
#include "manageEmul.h"
#include "manageSDL.h"
/*void interpret(struct Chip8 * chip8){
    struct cpu8 * cpu = &(chip8->cpu);

    if(!(chip8->cpu.waitingFX0A)){
        uint16_t opCode = getOpCode(cpu);

        uint8_t b1 = opCode & 0x00F;
        uint8_t b2 = (opCode & 0x0F0) >> 4;
        uint8_t b3 = (opCode & 0xF00) >> 8;
        
        uint8_t action = getAction(&(chip8->jumpTable), opCode);
        printf("%d\n", action);

        if(action < OPCODE_NB && action>0)
            chip8->opFunction[action](chip8, b1, b2, b3);  

        cpu->cur += 2;
    }
    else{
        chip8->opFunction[27](chip8, cpu->vx, 0x0, 0xA);
    }
}


void emulateChip8(struct Chip8 * chip8){
    uint32_t opCode = 0;
    while(chip8->control.quit == SDL_FALSE){
        updateEvent(&(chip8->control));
        analyseEvent(chip8);
        updateInput(chip8);

        interpret(chip8);

        SDL_Delay(FREQUENCY);
        decrement(&(chip8->cpu));

        if(SDL_TICKS_PASSED(SDL_GetTicks(), chip8->screen.nextRefresh)){
            renderScreen(&(chip8->screen));
            chip8->screen.nextRefresh = SDL_GetTicks() + FPS;
        }
    }
}*/
/*
for emulation : 
1 instruction :
vblank flag take down
9 other instruction (or more or less see depending on the refreshing rate and frequency of cpu)
*/

int fetchOpcode(struct Chip16 * chip16, uint8_t * a, uint8_t * b, uint8_t * c){
    struct cpu16 * cpu = &(chip16->cpu);

    /*uint32_t opcode = ((uint32_t) cpu->mem[cpu->pc])
                    | (((uint32_t) cpu->mem[cpu->pc+1])<<8)
                    | (((uint32_t) cpu->mem[cpu->pc+2])<<16)
                    | (((uint32_t) cpu->mem[cpu->pc+3])<<24);*/
    uint8_t id = cpu->mem[cpu->pc];
    *a = cpu->mem[cpu->pc+1];
    *b = cpu->mem[cpu->pc+2];
    *c = cpu->mem[cpu->pc+3];
    cpu->pc += 4;

    int i;
    for(i=0; i<OPCODE_NB; i++){
        if(chip16->jumpTable.id[i] == id){
            break;
        }
    }
    
    if(i == OPCODE_NB || id == 0){
        fprintf(stderr, "Error : unknown opcode %d\n", id);
        fprintf(stderr, "Old opcode : %hhX\n", chip16->cpu.mem[chip16->cpu.pc-8]);
    }
    //fprintf(stderr, "Opcode %hhX\n", id);
    return i;
}

void interpret(struct Chip16 * chip16){
    uint8_t a, b, c;

    int opNb = fetchOpcode(chip16, &a, &b, &c);

    chip16->opcodeFcts[opNb](chip16, a, b, c);
}

void emulateChip16(struct Chip16 * chip16){
    int nbOp = FREQUENCY * SPF;

    while(chip16->control.quit == SDL_FALSE){
        updateControl(chip16);
        analyseEvent(chip16);
        //fprintf(stderr, "%d\n", *(chip16->control.control1));

        interpret(chip16);
        chip16->screen.reg.vblank = false;

        for(int i=1; i<nbOp; i++){
            interpret(chip16);
        }

        while(!(SDL_TICKS_PASSED(SDL_GetTicks(), chip16->screen.nextRefresh))){

        }

        renderScreen(&(chip16->screen));
        chip16->screen.nextRefresh = SDL_GetTicks() + SPF;

    }
}