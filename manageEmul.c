#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "emul.h"
#include "manageSDL.h"
#include "managePa.h"
#include "opFunction.h"

void clearScreen(struct Screen * screen){
    screen->bg = 0;
    memset((screen->fg), 0x00, sizeof(screen->fg));
}

void drawTestScreen(struct Screen * screen){
    for(int i=1; i<PIXEL_BY_WIDTH; i++){
        for(int j =1; j<PIXEL_BY_HEIGHT; j++){
            screen->fg[i][j] = j%0x10;
        }
    }

}
void initCPU(struct cpu16 * cpu){
    memset(cpu->mem, 0, sizeof(cpu->mem));
    memset(cpu->reg, 0, sizeof(cpu->reg));

    cpu->pc = 0;

    cpu->C = 0;
    cpu->Z = 0;
    cpu->N = 0;
    cpu->O = 0;
    
    cpu->sp = STACK_INIT_ADD;
}



SDL_Color hexToRGB(uint32_t hex){
    SDL_Color color;
    color.r = (hex & 0x00FF0000)>>16;
    color.g = (hex & 0x0000FF00)>>8;
    color.b = (hex & 0x000000FF);
    color.a = 0;
    return color;
}

void initPalette(SDL_Color * palette){
    palette[0] = hexToRGB(0x00000000);
    palette[1] = hexToRGB(0x00000000);
    palette[2] = hexToRGB(0x00888888);
    palette[3] = hexToRGB(0x00BF3932);
    palette[4] = hexToRGB(0x00DE7AAE);
    palette[5] = hexToRGB(0x004C3D21);
    palette[6] = hexToRGB(0x00905F25);
    palette[7] = hexToRGB(0x00E49452);
    palette[8] = hexToRGB(0x00EAD979);
    palette[9] = hexToRGB(0x00537A3B);
    palette[10] = hexToRGB(0x00ABD54A);
    palette[11] = hexToRGB(0x00252E38);
    palette[12] = hexToRGB(0x0000467F);
    palette[13] = hexToRGB(0x0068ABCC);
    palette[14] = hexToRGB(0x00BCDEE4);
    palette[15] = hexToRGB(0x00FFFFFF);
}

int initScreen(struct Screen *screen){
    if(initScreenSDL(screen)!=EXIT_SUCCESS){
        return EXIT_FAILURE;
    }
    
    clearScreen(screen);
    screen->pixelHeight = PIXEL_DIM;
    screen->pixelWidth = PIXEL_DIM;

    memset(&(screen->reg), 0, sizeof((screen->reg)));
    
    initPalette(screen->palette);

    screen->nextRefresh = SDL_GetTicks() + SPF;

    return EXIT_SUCCESS;
}

void initSoundTable(struct Sound * sound){
    sound->attackTable[0] = 2;
    sound->attackTable[1] = 8;
    sound->attackTable[2] = 16;
    sound->attackTable[3] = 24;
    sound->attackTable[4] = 38;
    sound->attackTable[5] = 56;
    sound->attackTable[6] = 68;
    sound->attackTable[7] = 80;
    sound->attackTable[8] = 100;
    sound->attackTable[9] = 250;
    sound->attackTable[10] = 500;
    sound->attackTable[11] = 800;
    sound->attackTable[12] = 1000;
    sound->attackTable[13] = 3000;
    sound->attackTable[14] = 5000;
    sound->attackTable[15] = 8000;

    sound->decayTable[0] = 6;
    sound->decayTable[1] = 24;
    sound->decayTable[2] = 48;
    sound->decayTable[3] = 72;
    sound->decayTable[4] = 114;
    sound->decayTable[5] = 168;
    sound->decayTable[6] = 204;
    sound->decayTable[7] = 240;
    sound->decayTable[8] = 300;
    sound->decayTable[9] = 750;
    sound->decayTable[10] = 1500;
    sound->decayTable[11] = 2400;
    sound->decayTable[12] = 3000;
    sound->decayTable[13] = 9000;
    sound->decayTable[14] = 15000;
    sound->decayTable[15] = 24000;

    sound->releaseTable[0] = 6;
    sound->releaseTable[1] = 24;
    sound->releaseTable[2] = 48;
    sound->releaseTable[3] = 72;
    sound->releaseTable[4] = 114;
    sound->releaseTable[5] = 168;
    sound->releaseTable[6] = 204;
    sound->releaseTable[7] = 240;
    sound->releaseTable[8] = 300;
    sound->releaseTable[9] = 750;
    sound->releaseTable[10] = 1500;
    sound->releaseTable[11] = 2400;
    sound->releaseTable[12] = 3000;
    sound->releaseTable[13] = 9000;
    sound->releaseTable[14] = 15000;
    sound->releaseTable[15] = 24000;
}

int initSound(struct Sound * sound){
    memset(sound, 0, sizeof(*sound));

    //Setting the pointer to NULL specifically
    sound->stream = NULL;
    sound->streamInfo = NULL;
    sound->wave = NULL;

    if(initPa(sound, 1) !=  EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    initSoundTable(sound);
    return EXIT_SUCCESS;
}

void initKeyTable(struct Control * control){
    SDL_Scancode * table = control->keyTable;
    table[0] = SDL_SCANCODE_UP; //UP
    table[1] = SDL_SCANCODE_DOWN; //DOWN
    table[2] = SDL_SCANCODE_LEFT; //LEFT
    table[3] = SDL_SCANCODE_RIGHT; //RIGHT
    table[4] = SDL_SCANCODE_KP_0; //SELECT
    table[5] = SDL_SCANCODE_KP_ENTER; //START
    table[6] = SDL_SCANCODE_KP_1; //A
    table[7] = SDL_SCANCODE_KP_2; //B
    table[8] = SDL_SCANCODE_Z;
    table[9] = SDL_SCANCODE_S;
    table[10] = SDL_SCANCODE_Q;
    table[11] = SDL_SCANCODE_D;
    table[12] = SDL_SCANCODE_A;
    table[13] = SDL_SCANCODE_E;
    table[14] = SDL_SCANCODE_F;
    table[15] = SDL_SCANCODE_G;
}

void initControl(struct Control * control, struct cpu16 * cpu){
    memset(control, SDL_FALSE, sizeof(control->key));
    control->quit = SDL_FALSE;
    control->resizeWindow = SDL_FALSE;

    initKeyTable(control);

    control->control1 = (uint16_t *) &(cpu->mem[CONTROL1_ADD]);
    control->control2 = (uint16_t *) &(cpu->mem[CONTROL2_ADD]);
}


void updateControl(struct Chip16 * chip){
    struct Control * control = &(chip->control);

    //Updating the SDL Event 
    updateEvent(control); 

    //Clearing the input register
    *(control->control1) = 0x0000;
    *(control->control2) = 0x0000;
    
    //Updating the input register
    for(int i = 0; i<INPUT_NB/2; i++){
        if(control->key[control->keyTable[i]] == SDL_TRUE){
            *(control->control1) = (0b0000000000000001)<<i;
        }
    }

    for(int i = 0; i<INPUT_NB/2; i++){
        if(control->key[control->keyTable[i+8]] == SDL_TRUE){
            *(control->control2) = (0b0000000000000001)<<i;
        }
    }
}

void resizeWindow(struct Screen * screen){
    int w, h;
    SDL_GetWindowSize(screen->window, &w, &h);
    screen->pixelWidth = w/PIXEL_BY_WIDTH;
    screen->pixelHeight = h/PIXEL_BY_HEIGHT;
    SDL_SetWindowSize(screen->window, screen->pixelWidth * PIXEL_BY_WIDTH, screen->pixelHeight * PIXEL_BY_HEIGHT);
}

void analyseEvent(struct Chip16 * chip16){ //Analyze event relevent to the emulator itself
    if(chip16->control.resizeWindow == SDL_TRUE){
        resizeWindow(&(chip16->screen));
    }
    if(chip16->control.key[SDL_SCANCODE_O] == SDL_TRUE){
        renderScreen(&(chip16->screen));
    }
}

size_t checkHeader(struct cpu16 * cpu, FILE * file){ //Return 0 in case of non success, the size of the ROM otherwize
    char magicNumber[5];
    memset(magicNumber, '\0', sizeof(magicNumber));

    if(fread(magicNumber, 1, 4, file)!=4){
        return 0;
    }
    
    if(strcmp(magicNumber, "CH16")!=0){
        fprintf(stderr, "No metadata");
        return 0; //Non presence of header
    }


    //Presence of header
    if(fseek(file, 2, SEEK_CUR)!=0){
        fprintf(stderr, "Error while loading metadata");
        return 0;
    }
    size_t size;
    if(fread(&size, 4, 1, file)!=1){
        fprintf(stderr, "Error while loading metadata");
        return 0;
    }
    if(fread(&(cpu->pc), 2, 1, file)!=1){
        fprintf(stderr, "Error while loading metadata");
        return 0;
    }
    if(fseek(file, 4, SEEK_CUR)!=0){
        fprintf(stderr, "Error while loading metadata");
        return 0;
    }

    return size;
}   

int loadROM(struct cpu16 * cpu, char * path){
    int ret = EXIT_FAILURE;
    FILE * rom = fopen(path, "r");
    if(rom == NULL){
        perror("fopen");
        goto loadROMend;
    }
    size_t size = checkHeader(cpu, rom);
    if(size == 0){
        for(size_t i = CURSOR0; i<MEMORY_SIZE; i++){
            if(fread(&(cpu->mem[i]), sizeof(uint8_t), 1, rom) != 1){
                fprintf(stderr, "Error reading rom, %s\n", path);
                goto loadROMend;
            }
            //printf("%d\n", cpu->mem[i]);
        }
    }
    else{
        fprintf(stderr, "Found header size = %ld\n", size);
        if(fread(cpu->mem, sizeof(uint8_t), size, rom)!=size){
            fprintf(stderr, "Error while reading ROM\n");
            return EXIT_FAILURE;
        }
    }
    ret = EXIT_SUCCESS;
loadROMend:
    if(rom != NULL && fclose(rom) == EOF){
        ret = EXIT_FAILURE;
    }
    return ret;
}

void initJumpTable(struct SJump * jump){
    jump->mask =  0xFF000000;

    jump->id[0] = 0x00;
    jump->id[1] = 0x01;
    jump->id[2] = 0x02;
    jump->id[3] = 0x03;
    jump->id[4] = 0x04;
    jump->id[5] = 0x05;
    jump->id[6] = 0x06;
    jump->id[7] = 0x07;
    jump->id[8] = 0x08;
    jump->id[9] = 0x09;
    jump->id[10] = 0x0A;
    jump->id[11] = 0x0B;
    jump->id[12] = 0x0C;
    jump->id[13] = 0x0D;
    jump->id[14] = 0x0E;

    jump->id[15] = 0x10;
    jump->id[16] = 0x11;
    jump->id[17] = 0x12;
    jump->id[18] = 0x13;
    jump->id[19] = 0x14;
    jump->id[20] = 0x15;

    jump->id[21] = 0x20;
    jump->id[22] = 0x21;
    jump->id[23] = 0x22;
    jump->id[24] = 0x23;
    jump->id[25] = 0x24;

    jump->id[26] = 0x30;
    jump->id[27] = 0x31;

    jump->id[28] = 0x40;
    jump->id[29] = 0x41;
    jump->id[30] = 0x42;

    jump->id[31] = 0x50;
    jump->id[32] = 0x51;
    jump->id[33] = 0x52;
    jump->id[34] = 0x53;
    jump->id[35] = 0x54;

    jump->id[36] = 0x60;
    jump->id[37] = 0x61;
    jump->id[38] = 0x62;
    jump->id[39] = 0x63;

    jump->id[40] = 0x70;
    jump->id[41] = 0x71;
    jump->id[42] = 0x72;

    jump->id[43] = 0x80;
    jump->id[44] = 0x81;
    jump->id[45] = 0x82;

    jump->id[46] = 0x90;
    jump->id[47] = 0x91;
    jump->id[48] = 0x92;         

    jump->id[49] = 0xA0;
    jump->id[50] = 0xA1;
    jump->id[51] = 0xA2;
    jump->id[52] = 0xA3;
    jump->id[53] = 0xA4;
    jump->id[54] = 0xA5;
    jump->id[55] = 0xA6;
    jump->id[56] = 0xA7;

    jump->id[57] = 0xB0;
    jump->id[58] = 0xB1;
    jump->id[59] = 0xB2;
    jump->id[60] = 0xB3;
    jump->id[61] = 0xB4;
    jump->id[62] = 0xB5;

    jump->id[63] = 0xC0;
    jump->id[64] = 0xC1;
    jump->id[65] = 0xC2;
    jump->id[66] = 0xC3;
    jump->id[67] = 0xC4;
    jump->id[68] = 0xC5;

    jump->id[69] = 0xD0;
    jump->id[70] = 0xD1;

    jump->id[71] = 0xE0;
    jump->id[72] = 0xE1;
    jump->id[73] = 0xE2;
    jump->id[74] = 0xE3;
    jump->id[75] = 0xE4;
    jump->id[76] = 0xE5;

    //Forgotten function added after finishing
    jump->id[77] = 0x16;
    jump->id[78] = 0x17;
    jump->id[79] = 0x18;
}

void initOpcodeFcts(struct Chip16 * chip16){
    chip16->opcodeFcts[0] = opcode_00_00_00_00;
    chip16->opcodeFcts[1] = opcode_01_00_00_00;
    chip16->opcodeFcts[2] = opcode_02_00_00_00;
    chip16->opcodeFcts[3] = opcode_03_00_0N_00;
    chip16->opcodeFcts[4] = opcode_04_00_LL_HH;
    chip16->opcodeFcts[5] = opcode_05_YX_LL_HH;
    chip16->opcodeFcts[6] = opcode_06_YX_0Z_00;
    chip16->opcodeFcts[7] = opcode_07_0X_LL_HH;
    chip16->opcodeFcts[8] = opcode_08_00_00_00;
    chip16->opcodeFcts[9] = opcode_09_00_00_00;
    chip16->opcodeFcts[10] = opcode_0A_00_LL_HH;
    chip16->opcodeFcts[11] = opcode_0B_00_LL_HH;
    chip16->opcodeFcts[12] = opcode_0C_00_LL_HH;
    chip16->opcodeFcts[13] = opcode_0D_0X_LL_HH;
    chip16->opcodeFcts[14] = opcode_0E_AD_SR_VT;

    chip16->opcodeFcts[15] = opcode_10_00_LL_HH;
    chip16->opcodeFcts[16] = opcode_11_00_LL_HH;
    chip16->opcodeFcts[17] = opcode_12_0x_LL_HH;
    chip16->opcodeFcts[18] = opcode_13_YX_LL_HH;
    chip16->opcodeFcts[19] = opcode_14_00_LL_HH;
    chip16->opcodeFcts[20] = opcode_15_00_00_00;
    
    chip16->opcodeFcts[21] = opcode_20_0X_LL_HH;
    chip16->opcodeFcts[22] = opcode_21_00_LL_HH;
    chip16->opcodeFcts[23] = opcode_22_0X_LL_HH;
    chip16->opcodeFcts[24] = opcode_23_YX_00_00;
    chip16->opcodeFcts[25] = opcode_24_YX_00_00;

    chip16->opcodeFcts[26] = opcode_30_0X_LL_HH;
    chip16->opcodeFcts[27] = opcode_31_YX_00_00;

    chip16->opcodeFcts[28] = opcode_40_0X_LL_HH;
    chip16->opcodeFcts[29] = opcode_41_YX_00_00;
    chip16->opcodeFcts[30] = opcode_42_YX_0Z_00;
    
    chip16->opcodeFcts[31] = opcode_50_0X_LL_HH;
    chip16->opcodeFcts[32] = opcode_51_YX_00_00;
    chip16->opcodeFcts[33] = opcode_52_YX_0Z_00;
    chip16->opcodeFcts[34] = opcode_53_0X_LL_HH;
    chip16->opcodeFcts[35] = opcode_54_YX_00_00;
    
    chip16->opcodeFcts[36] = opcode_60_0X_LL_HH;
    chip16->opcodeFcts[37] = opcode_61_YX_00_00;
    chip16->opcodeFcts[38] = opcode_62_YX_0Z_00;
    chip16->opcodeFcts[39] = opcode_63_0X_LL_HH;
    
    chip16->opcodeFcts[40] = opcode_70_0X_LL_HH;
    chip16->opcodeFcts[41] = opcode_71_YX_00_00;
    chip16->opcodeFcts[42] = opcode_72_YX_0Z_00;
    
    chip16->opcodeFcts[43] = opcode_80_0X_LL_HH;
    chip16->opcodeFcts[44] = opcode_81_YX_00_00;
    chip16->opcodeFcts[45] = opcode_82_YX_0Z_00;
    
    chip16->opcodeFcts[46] = opcode_90_0X_LL_HH;
    chip16->opcodeFcts[47] = opcode_91_YX_00_00;
    chip16->opcodeFcts[48] = opcode_92_YX_0Z_00;

    chip16->opcodeFcts[49] = opcode_A0_0X_LL_HH;
    chip16->opcodeFcts[50] = opcode_A1_YX_00_00;
    chip16->opcodeFcts[51] = opcode_A2_YX_0Z_00;
    chip16->opcodeFcts[52] = opcode_A3_0X_LL_HH;
    chip16->opcodeFcts[53] = opcode_A4_YX_00_00;
    chip16->opcodeFcts[54] = opcode_A5_YX_0Z_00;
    chip16->opcodeFcts[55] = opcode_A6_0X_LL_HH;
    chip16->opcodeFcts[56] = opcode_A7_YX_00_00;

    chip16->opcodeFcts[57] = opcode_B0_0X_0N_00;
    chip16->opcodeFcts[58] = opcode_B1_0X_0N_00;
    chip16->opcodeFcts[59] = opcode_B2_0X_0N_00;
    chip16->opcodeFcts[60] = opcode_B3_YX_00_00;
    chip16->opcodeFcts[61] = opcode_B4_YX_00_00;
    chip16->opcodeFcts[62] = opcode_B5_YX_00_00;

    chip16->opcodeFcts[63] = opcode_C0_0X_00_00;
    chip16->opcodeFcts[64] = opcode_C1_0X_00_00;
    chip16->opcodeFcts[65] = opcode_C2_00_00_00;
    chip16->opcodeFcts[66] = opcode_C3_00_00_00;
    chip16->opcodeFcts[67] = opcode_C4_00_00_00;
    chip16->opcodeFcts[68] = opcode_C5_00_00_00;

    chip16->opcodeFcts[69] = opcode_D0_00_LL_HH;
    chip16->opcodeFcts[70] = opcode_D1_0X_00_00;

    chip16->opcodeFcts[71] = opcode_E0_0X_LL_HH;
    chip16->opcodeFcts[72] = opcode_E1_0X_00_00;
    chip16->opcodeFcts[73] = opcode_E2_YX_00_00;
    chip16->opcodeFcts[74] = opcode_E3_0X_LL_HH;
    chip16->opcodeFcts[75] = opcode_E4_0X_00_00;
    chip16->opcodeFcts[76] = opcode_E5_YX_00_00;

    chip16->opcodeFcts[77] = opcode_16_0X_00_00;
    chip16->opcodeFcts[78] = opcode_17_0x_LL_HH;
    chip16->opcodeFcts[79] = opcode_18_0X_00_00;
}

int initChip16(struct Chip16 * chip16){
    int ret = EXIT_FAILURE;
    
    initCPU(&(chip16->cpu));

    if(initScreen(&(chip16->screen)) != EXIT_SUCCESS) goto initChip16End;

    if(initSound(&(chip16->sound)) != EXIT_SUCCESS) goto initChip16End;

    initControl(&(chip16->control), &(chip16->cpu));

    initJumpTable(&(chip16->jumpTable));

    initOpcodeFcts(chip16);

    ret = EXIT_SUCCESS;
initChip16End:
    return ret;
}

