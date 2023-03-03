#ifndef MANAGE_EMUL_H
#define MANAGE_EMUL_H

int initChip16(struct Chip16 * chip16);
void drawTestScreen(struct Screen * screen);
void analyseEvent(struct Chip16 * chip16);
void updateControl(struct Chip16 * chip);
SDL_Color hexToRGB(uint32_t hex);
void clearScreen(struct Screen * screen);
int loadROM(struct cpu16 * cpu, char * path);

#endif