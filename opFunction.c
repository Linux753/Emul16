#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <portaudio.h>
#include "emul.h"
#include "managePa.h"
#include "manageEmul.h"

void opcode_00_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    return;
}

void opcode_01_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    clearScreen(&(chip16->screen));
}

void opcode_02_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    if(!chip16->screen.reg.vblank){
        chip16->cpu.pc -= 4;
    }
}

void opcode_03_00_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->screen.bg = (b & 0x0F);
}

void opcode_04_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->screen.reg.spritew = b;
    chip16->screen.reg.spriteh = c;
}

void drawPixel(struct Chip16 * chip16, int16_t x, int16_t y, uint8_t px){
    if(chip16->screen.fg[x][y] != 0 && px != 0){
        chip16->cpu.C = 1;
    }
    if(px != 0) chip16->screen.fg[x][y] = px;
}

void opcode_05_YX_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    struct Screen *screen = &(chip16->screen);

    int16_t y = chip16->cpu.reg[a>>4];
    int16_t x = chip16->cpu.reg[a&0x0F];
    uint16_t add = (((uint16_t) c )<<8) | ((uint16_t) b);

    chip16->cpu.C = 0;

    //iS and jS : screen coordinate : always go throught in the same order
    //iA and jA : adresses coordinate : decrement or increment depending on the hflip and vflip flags

    for(int jS = 0, jA = screen->reg.vflip? screen->reg.spriteh-1 : 0;
        screen->reg.vflip? jA>=0: jA<screen->reg.spriteh; 
        jS++, jA+= screen->reg.vflip? -1: 1){
        
        for(int iS = 0, iA = screen->reg.hflip? screen->reg.spritew-1: 0; 
        screen->reg.hflip? iA>=0: iA<chip16->screen.reg.spritew; 
        iS+=2,iA+= screen->reg.hflip? -1: +1){
            uint8_t dualPx = chip16->cpu.mem[add + jA*chip16->screen.reg.spritew + iA];

            if(y+jS>0 && y+jS<PIXEL_BY_HEIGHT){
                if(x+iS+1>0 && x+iS+1<PIXEL_BY_WIDTH){
                    if(screen->fg[x+iS+1][y+jS] != 0 && (dualPx & 0x0F) != 0){
                        chip16->cpu.C = 1;
                    }
                    if((dualPx&0x0F)!=0) chip16->screen.fg[x+iS+1][y+jS] = dualPx & 0x0F;
                    
                    //drawPixel(chip16, x+iS, y+iS, dualPx&0x0F);
                }
                if(x+iS>0 && x+iS<PIXEL_BY_WIDTH){
                    if(screen->fg[x+iS][y+jS] != 0 && (dualPx>>4) != 0){
                        chip16->cpu.C = 1;
                    }
                    if(dualPx>>4 != 0) chip16->screen.fg[x+iS][y+jS] = dualPx>>4;
                    //drawPixel(chip16, x+iS+1, y+iS, dualPx>>4);
                }
            }
        }
    }
}

void opcode_06_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    struct Screen * screen = &(chip16->screen);
    
    int16_t y = chip16->cpu.reg[a>>4];
    int16_t x = chip16->cpu.reg[a&0x0F];
    uint16_t add = chip16->cpu.reg[b&0x0F];

    chip16->cpu.C = 0;

    //iS and jS : screen coordinate : always go throught in the same order
    //iA and jA : adresses coordinate : decrement or increment depending on the hflip and vflip flags

    for(int jS = 0, jA = screen->reg.vflip? screen->reg.spriteh-1 : 0;
        //screen->reg.vflip? jA>=0: jA<screen->reg.spriteh; 
        jS<screen->reg.spriteh;
        jS++, jA+= screen->reg.vflip? -1: 1){
        
        for(int iS = 0, iA = screen->reg.hflip? screen->reg.spritew-1: 0; 
        //screen->reg.hflip? iA>=0: iA<chip16->screen.reg.spritew; 
        iS<2*chip16->screen.reg.spritew;
        iS+=2,iA+= screen->reg.hflip? -1: +1){
            uint8_t dualPx = chip16->cpu.mem[add + jA*chip16->screen.reg.spritew + iA];
            
            if(y+jS>0 && y+jS<PIXEL_BY_HEIGHT){
                if(x+iS+1>0 && x+iS+1<PIXEL_BY_WIDTH){
                    if(screen->fg[x+iS+1][y+jS] != 0 && (dualPx & 0x0F) != 0){
                        chip16->cpu.C = 1;
                    }
                    if((dualPx&0x0F)!=0) chip16->screen.fg[x+iS+1][y+jS] = dualPx & 0x0F;
                    //drawPixel(chip16, x+iS, y+iS, dualPx&0x0F);
                }
                if(x+iS>0 && x+iS<PIXEL_BY_WIDTH){
                    if(screen->fg[x+iS][y+jS] != 0 && (dualPx>>4) != 0){
                        chip16->cpu.C = 1;
                    }
                    if(dualPx>>4 != 0) chip16->screen.fg[x+iS][y+jS] = dualPx>>4;
                    //drawPixel(chip16, x+iS+1, y+iS, dualPx>>4);
                }
            }
        }
    }
}

uint16_t randomMax(uint16_t max){
    return ((long)rand())%(((long)max)+1);
}

void opcode_07_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint16_t max = ((uint16_t) c)<<8 | ((uint16_t)b);
    chip16->cpu.reg[a&0x0F] = randomMax(max);
}

void opcode_08_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->screen.reg.vflip = (c & 0b01);
    chip16->screen.reg.hflip = (c & 0b10)>1;
}

void opcode_09_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    abortSoundStream(&(chip16->sound));
}

void opcode_0A_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    playTone(&(chip16->sound), 500, (((uint16_t) c)<<8) | ((uint16_t) b), false);
}

void opcode_0B_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    playTone(&(chip16->sound), 1000, (((uint16_t) c)<<8) | ((uint16_t) b), false);
}

void opcode_0C_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    playTone(&(chip16->sound), 1500, (((uint16_t) c)<<8) | ((uint16_t) b), false);
}

void opcode_0D_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    playTone(&(chip16->sound), chip16->cpu.reg[a], (((uint16_t) c)<<8) | ((uint16_t) b), true);
}

void opcode_0E_AD_SR_VT(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    struct Sound * sound = &(chip16->sound);
    
    uint8_t attackId = (a&0xF0)>>4;
    uint8_t decayId = (a&0x0F);
    
    uint8_t sustainId = (b&0xF0)>>4;    
    uint8_t releaseId = (b&0x0F);

    uint8_t volumeId = (c&0xF0)>>4;
    sound->type = (c&0x0F);

    //Time in second
    sound->attackEndTime = ((double)sound->attackTable[attackId])*0.001;
    sound->decayEndTime = ((double)sound->decayTable[decayId])*0.001 + sound->attackEndTime;
    sound->releaseBegTime = -((double)sound->releaseTable[releaseId])*0.001; //Relative begining time to the end of the tone

    //Indices in termes in the number of frames played
    sound->attackEndI = sound->streamInfo->sampleRate * sound->attackEndTime;
    sound->decayEndI = sound->streamInfo->sampleRate * sound->decayEndTime;
  
   //Waiting for the duration of the tone to set up the releaseBegId

    sound->volume = 0.125f * volumeId;
    sound->sustain = 0.125f * sustainId;

    //Slope of volume:
    if(sound->attackEndI !=0){
        sound->slopeAttack = sound->volume/sound->attackEndI;
    }
    else{
        sound->slopeAttack = 1;
    }
    if(sound->attackEndI - sound->decayEndI != 0){
        sound->slopeDecay = (sound->volume - sound->sustain)/(sound->attackEndI-sound->decayEndI);
    }
    else{
        sound->slopeDecay = 1;
    }
    if(sound->releaseBegI != 0){
        sound->slopeRelease = -sound->sustain/(sound->streamInfo->sampleRate * sound->releaseBegTime);
    }
    else{
        sound->slopeDecay = -1;
    }
}

void opcode_10_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.pc = b | (((uint16_t)c)<<8);
}

void opcode_11_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    if(chip16->cpu.C){
        chip16->cpu.pc = b | (((uint16_t)c)<<8);
    }
}

bool conditionnalJump(struct Chip16 * chip16, uint8_t x){
    struct cpu16 * cpu = &(chip16->cpu);
    bool jump = false;
    switch(x){
        case 0x0:
            jump = cpu->Z==1;
            break;
        case 0x1:
            jump = cpu->Z==0;
            break;
        case 0x2:
            jump = cpu->N==1;
            break;
        case 0x3:
            jump = cpu->N==0;
            break;
        case 0x4:
            jump = cpu->N==0 && cpu->Z==0;
            break;
        case 0x5:
            jump = cpu->O ==1;
            break;
        case 0x6:
            jump = cpu->O ==0;
            break;
        case 0x7:
            jump = cpu->C ==0 && cpu->Z ==0;
            break;
        case 0x8:
            jump = cpu->C ==0;
            break;
        case 0x9:
            jump = cpu->C ==1;
            break;
        case 0xA:
            jump = cpu->C == 1 || cpu->Z == 1;
            break;
        case 0xB:
            jump = cpu->O == cpu->N && cpu->Z ==0;
            break;
        case 0xC:
            jump = cpu->O == cpu->N;
            break;
        case 0xD:
            jump = cpu->O != cpu->N;
            break;
        case 0xE:
            jump = cpu->O != cpu->N || cpu->Z ==1;
            break;
    }
    return jump;
}

void opcode_12_0x_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    if(conditionnalJump(chip16, a)){
        chip16->cpu.pc = b | (((uint16_t)c)<<8);
    }
}

void opcode_13_YX_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    if(chip16->cpu.reg[x] == chip16->cpu.reg[y]){
        chip16->cpu.pc = b | (((uint16_t)c)<<8);
    }
}

void opcode_14_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.mem[chip16->cpu.sp] = chip16->cpu.pc & 0x00FF;
    chip16->cpu.mem[chip16->cpu.sp+1] = (chip16->cpu.pc & 0xFF00)>>8;

    chip16->cpu.sp +=2;

    chip16->cpu.pc = b | (((uint16_t) c)<<8); 
}  

void opcode_15_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.sp -=2;

    chip16->cpu.pc = chip16->cpu.mem[chip16->cpu.sp] | (((uint16_t) chip16->cpu.mem[chip16->cpu.sp+1])<<8);
}

void opcode_16_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.pc = chip16->cpu.reg[a];
}

void opcode_17_0x_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    if(conditionnalJump(chip16, a)){
        opcode_14_00_LL_HH(chip16, 0x00, b, c);
    }
}

void opcode_18_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.mem[chip16->cpu.sp] = chip16->cpu.pc & 0x00FF;
    chip16->cpu.mem[chip16->cpu.sp+1] = (chip16->cpu.pc & 0xFF00)>>8;

    chip16->cpu.sp +=2;

    chip16->cpu.pc = chip16->cpu.reg[a]; 
}

void opcode_20_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0X0F; //Theoricaly useless
    chip16->cpu.reg[x] = (((uint16_t)c)<<8) | b;
}

void opcode_21_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.sp =  (((uint16_t)c)<<8) | b;
}

void opcode_22_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F; //Theoricaly useless
    uint16_t add = ((uint16_t)c<<8) | b;
    chip16->cpu.reg[x] =  ((uint16_t) chip16->cpu.mem[add]) | (((uint16_t) chip16->cpu.mem[add +1])<<8);
}

void opcode_23_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t y = (a&0xF0)>>4;
    uint8_t x = (a&0x0F);
    uint16_t add = (uint16_t) chip16->cpu.reg[y];

    chip16->cpu.reg[x] = ((uint16_t) chip16->cpu.mem[add]) | (((uint16_t)chip16->cpu.mem[add+1])<<8);
}

void opcode_24_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t y = (a&0xF0)>>4;
    uint8_t x = (a&0x0F);
    chip16->cpu.reg[x] = chip16->cpu.reg[y];
}

void opcode_30_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0X0F; //Theorically useless (doesn't do it in the following)
    uint16_t add = (((uint16_t) c)<<8 )| b;
    
    chip16->cpu.mem[add] = (chip16->cpu.reg[x] & 0x00FF);
    chip16->cpu.mem[add+1] = (chip16->cpu.reg[x] & 0xFF00)>>8;
}

void opcode_31_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t y = (a&0xF0)>>4;
    uint8_t x = (a&0x0F);
    
    chip16->cpu.mem[(uint16_t) chip16->cpu.reg[y]] = (chip16->cpu.reg[x] & 0x00FF);
    chip16->cpu.mem[(uint16_t) chip16->cpu.reg[y]+1] = (chip16->cpu.reg[x] & 0xFF00)>>8;

}

void opcode_40_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    
    int16_t nb1;
    uint16_t value = ((((uint16_t) c)<<8 )| b);
    memcpy(&nb1, &value, sizeof(nb1));
    int16_t nb2 = chip16->cpu.reg[a];

    chip16->cpu.C = __builtin_add_overflow(nb2, nb1, &(chip16->cpu.reg[a]))? 1:0;
    
    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;

    if((chip16->cpu.reg[a]>0&& nb1<0 && nb2<0)
    ||(chip16->cpu.reg[a]<0 && nb1>0 && nb2 > 0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }

}

void opcode_41_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    int16_t nb1 = chip16->cpu.reg[x];
    int16_t nb2 = chip16->cpu.reg[y];

    chip16->cpu.C = __builtin_add_overflow(nb2, nb1, &(chip16->cpu.reg[x]))? 1:0;
    
    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;

    if((chip16->cpu.reg[x]>0&& nb1<0 && nb2<0)
    ||(chip16->cpu.reg[x]<0 && nb1>0 && nb2 > 0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_42_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = (b&0X0F);

    int16_t nb1 = chip16->cpu.reg[x];
    int16_t nb2 = chip16->cpu.reg[y];

    chip16->cpu.C = __builtin_add_overflow(nb2, nb1, &(chip16->cpu.reg[z]))? 1:0;

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z]<0? 1:0;

    if((chip16->cpu.reg[z]>0&& nb1<0 && nb2<0)
    ||(chip16->cpu.reg[z]<0 && nb1>0 && nb2 > 0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_50_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    int16_t nb2 = (((uint16_t) c)<<8 )| b;
    int16_t nb1 = chip16->cpu.reg[a];

    chip16->cpu.C = __builtin_sub_overflow(nb1, nb2, &(chip16->cpu.reg[a]))? 1:0;
    
    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
    
    //WARNING THIS IS AN APPROXIMATION MAYBE I DON'T REALLY UNDERSTAND WHAT BORROW MEAN IN THIS CONTEXT
    chip16->cpu.C = chip16->cpu.N;
    if((chip16->cpu.reg[a]>0&& nb1<0 && nb2>0)
    ||(chip16->cpu.reg[a]<0 && nb1>0 && nb2<0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_51_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    int16_t nb1 = chip16->cpu.reg[x];
    int16_t nb2 = chip16->cpu.reg[y];

    chip16->cpu.C = __builtin_sub_overflow(nb1, nb2, &(chip16->cpu.reg[x]))? 1:0;
    
    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
    
    chip16->cpu.C = chip16->cpu.N;

    if((chip16->cpu.reg[x]>0&& nb1<0 && nb2>0)
    ||(chip16->cpu.reg[x]<0 && nb1>0 && nb2<0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_52_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = (b&0X0F);

    int16_t nb1 = chip16->cpu.reg[x];
    int16_t nb2 = chip16->cpu.reg[y];

    chip16->cpu.C = __builtin_sub_overflow(nb1, nb2, &(chip16->cpu.reg[z]))? 1:0;

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z]<0? 1:0;
    chip16->cpu.C = chip16->cpu.N;

    if((chip16->cpu.reg[z]>0&& nb1<0 && nb2>0)
    ||(chip16->cpu.reg[z]<0 && nb1>0 && nb2<0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_53_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    int16_t nb2 = (((uint16_t) c)<<8 )| b;
    int16_t nb1 = chip16->cpu.reg[a];
    int16_t result;

    chip16->cpu.C = __builtin_sub_overflow(nb1, nb2, &result)? 1:0;
    
    chip16->cpu.Z = result == 0? 1:0;
    chip16->cpu.N = result<0? 1:0;
    
    chip16->cpu.C = chip16->cpu.N;

    if((result>0&& nb1<0 && nb2>0)
    ||(result<0 && nb1>0 && nb2<0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_54_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    int16_t nb1 = chip16->cpu.reg[x];
    int16_t nb2 = chip16->cpu.reg[y];
    int16_t result;

    chip16->cpu.C = __builtin_sub_overflow(nb1, nb2, &result)? 1:0;
    
    chip16->cpu.Z = result == 0? 1:0;
    chip16->cpu.N = result<0? 1:0;
    
    chip16->cpu.C = chip16->cpu.N;

    if((result>0&& nb1<0 && nb2>0)
    ||(result<0 && nb1>0 && nb2<0)){
        chip16->cpu.O = 1;
    }
    else{
        chip16->cpu.O = 0;
    }
}

void opcode_60_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] &= (((uint16_t) c)<<8 )| b;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_61_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] &= chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}

void opcode_62_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = b&0x0F;

    chip16->cpu.reg[z] = chip16->cpu.reg[x]&chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z] <0? 1:0;
}

void opcode_63_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    int16_t result = chip16->cpu.reg[a] & ((((uint16_t) c)<<8 )| b);

    chip16->cpu.Z = result == 0? 1:0;
    chip16->cpu.N = result <0? 1:0;
}

void opcode_64_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    int16_t result = chip16->cpu.reg[x] & chip16->cpu.reg[y];

    chip16->cpu.Z = result == 0? 1:0;
    chip16->cpu.N = result <0? 1:0;
}

void opcode_70_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] |= (((uint16_t) c)<<8 )| b;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_71_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] |= chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}

void opcode_72_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = b&0x0F;

    chip16->cpu.reg[z] = chip16->cpu.reg[x] | chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z] <0? 1:0;
}

void opcode_80_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] ^= (((uint16_t) c)<<8 )| b;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_81_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] ^= chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}

void opcode_82_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = b&0x0F;

    chip16->cpu.reg[z] = chip16->cpu.reg[x] ^ chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z] <0? 1:0;
}

void opcode_90_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.C = __builtin_mul_overflow(chip16->cpu.reg[a], (((uint16_t) c)<<8 )| b, &(chip16->cpu.reg[a]));

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_91_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.C = __builtin_mul_overflow(chip16->cpu.reg[x],chip16->cpu.reg[y], &(chip16->cpu.reg[x]));

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}

void opcode_92_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = b&0x0F;

    chip16->cpu.C = __builtin_mul_overflow(chip16->cpu.reg[x], chip16->cpu.reg[y] ,&(chip16->cpu.reg[z]));

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z] <0? 1:0;
}

void opcode_A0_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.C = chip16->cpu.reg[a]%((((uint16_t) c)<<8 )| b) !=0 ? 1:0;  

    chip16->cpu.reg[a] = chip16->cpu.reg[a]/((c<<8) | b);

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_A1_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;
    
    chip16->cpu.C = chip16->cpu.reg[x]%chip16->cpu.reg[y] ? 1:0;  

    chip16->cpu.reg[x] = chip16->cpu.reg[x]/chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

void opcode_A2_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = (b&0x0F);

    chip16->cpu.C = chip16->cpu.reg[x]%chip16->cpu.reg[y] ? 1:0;  

    chip16->cpu.reg[z] = chip16->cpu.reg[x]/chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z]<0? 1:0;
}

int16_t modulo(int16_t a, int16_t b){
    return (a%b>=0 || b<0)? a%b: a%b + b; 
}

void opcode_A3_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = modulo(chip16->cpu.reg[a], (((uint16_t) c)<<8 )| b);

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_A4_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] = modulo(chip16->cpu.reg[x], chip16->cpu.reg[y]);

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

void opcode_A5_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;
    uint8_t z = (b&0x0F);

    chip16->cpu.reg[z] = modulo(chip16->cpu.reg[x],chip16->cpu.reg[y]);

    chip16->cpu.Z = chip16->cpu.reg[z] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[z]<0? 1:0;
}

void opcode_A6_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = chip16->cpu.reg[a]%((((uint16_t) c)<<8 )| b);

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_A7_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] = chip16->cpu.reg[x]%chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

 

void opcode_B0_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = chip16->cpu.reg[a]<<b;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_B1_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = chip16->cpu.reg[a]>>b;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_B2_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = chip16->cpu.reg[a]>>b;

    chip16->cpu.reg[a] |= (chip16->cpu.reg[a] & 0b0100000000000000) << 1;

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a]<0? 1:0;
}

void opcode_B3_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] = chip16->cpu.reg[x]<<chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

void opcode_B4_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] = chip16->cpu.reg[x]>>chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

void opcode_B5_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = (a&0x0F);
    uint8_t y = (a&0xF0)>>4;

    chip16->cpu.reg[x] = chip16->cpu.reg[x]>>chip16->cpu.reg[y];

    chip16->cpu.reg[x] |= (chip16->cpu.reg[x] & 0b0100000000000000) << 1;

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x]<0? 1:0;
}

void opcode_C0_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.mem[chip16->cpu.sp] = chip16->cpu.reg[a]&0x00FF;
    chip16->cpu.mem[chip16->cpu.sp+1] = (chip16->cpu.reg[a]&0xFF00) >>8;
    chip16->cpu.sp+=2;
}

void opcode_C1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.sp-=2;
    chip16->cpu.reg[a] = chip16->cpu.mem[chip16->cpu.sp] | (((int16_t) chip16->cpu.mem[chip16->cpu.sp+1])<<8);
}

void opcode_C2_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    for(uint8_t x=0; x<=0xF; x++){
        opcode_C0_0X_00_00(chip16, x, 0, 0);
    }
}

void opcode_C3_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    for(uint8_t x=0x10; x>0; x--){
        opcode_C1_0X_00_00(chip16, x-1, 0, 0);
    }
}

void opcode_C4_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.mem[chip16->cpu.sp] = chip16->cpu.C<<1 
                                    | chip16->cpu.Z<<2
                                    | chip16->cpu.O<<6
                                    | chip16->cpu.N<<7;
    chip16->cpu.sp += 2;
}

void opcode_C5_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.sp -= 2;
    
    chip16->cpu.C = (chip16->cpu.mem[chip16->cpu.sp] & 0b00000010)>>1;
    chip16->cpu.Z = (chip16->cpu.mem[chip16->cpu.sp] & 0b00000100)>>2;
    chip16->cpu.O = (chip16->cpu.mem[chip16->cpu.sp] & 0b01000000)>>6;
    chip16->cpu.N = (chip16->cpu.mem[chip16->cpu.sp] & 0b10000000)>>7;
}

void opcode_D0_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    struct cpu16 * cpu = &(chip16->cpu);
    uint16_t add = (((uint16_t) c)<<8) | b;
    
    for(int i=0; i<=0xF; i++){
        uint32_t colorHex = cpu->mem[add] 
                            | (((uint32_t) cpu->mem[add+1])<<8)
                            | (((uint32_t) cpu->mem[add+2])<<16)
                            | (((uint32_t) cpu->mem[add+3])<<24);
        chip16->screen.palette[i] = hexToRGB(colorHex);
        add+=4;
    }
}

void opcode_D1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    struct cpu16 * cpu = &(chip16->cpu);
    uint16_t add = cpu->reg[a];
    
    for(int i=0; i<=0xF; i++){
        uint32_t colorHex = cpu->mem[add] 
                            | (((uint32_t) cpu->mem[add+1])<<8)
                            | (((uint32_t) cpu->mem[add+2])<<16)
                            | (((uint32_t) cpu->mem[add+3])<<24);
        chip16->screen.palette[i] = hexToRGB(colorHex);
        add+=4;
    }
}

void opcode_E0_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = ~(b | (((int16_t) c)<<8));

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_E1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = ~chip16->cpu.reg[a];

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_E2_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0) >> 4;
    
    chip16->cpu.reg[x] = ~chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}

void opcode_E3_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = -((int16_t) (b | (((int16_t) c)<<8)));

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_E4_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    chip16->cpu.reg[a] = -chip16->cpu.reg[a];

    chip16->cpu.Z = chip16->cpu.reg[a] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[a] <0? 1:0;
}

void opcode_E5_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c){
    uint8_t x = a&0x0F;
    uint8_t y = (a&0xF0) >> 4;
    
    chip16->cpu.reg[x] = -chip16->cpu.reg[y];

    chip16->cpu.Z = chip16->cpu.reg[x] == 0? 1:0;
    chip16->cpu.N = chip16->cpu.reg[x] <0? 1:0;
}
