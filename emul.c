#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "emul.h"
#include "manageEmul.h"
#include "manageSDL.h"

int fetchOpcode(struct Chip16 * chip16, uint8_t * a, uint8_t * b, uint8_t * c){
    struct cpu16 * cpu = &(chip16->cpu);

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
    
    if(i == OPCODE_NB){
        fprintf(stderr, "Error : unknown opcode %d\n", id);
    }
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