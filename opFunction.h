#ifndef OP_FUNCTION_H
#define OP_FUNCTION_H

#include <stdint.h>
#include "emul.h"

void opcode_00_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_01_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_02_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_03_00_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_04_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_05_YX_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_06_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_07_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_08_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_09_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_0A_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_0B_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_0C_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_0D_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_0E_AD_SR_VT(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_10_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_11_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_12_0x_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_13_YX_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_14_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_15_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
//Function added after finishing 
void opcode_16_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_17_0x_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_18_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_20_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_21_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_22_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_23_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_24_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_30_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_31_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_40_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_41_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_42_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_50_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_51_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_52_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_53_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_54_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_60_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_61_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_62_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_63_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_70_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_71_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_72_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_80_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_81_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_82_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_90_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_91_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_92_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_A0_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A1_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A2_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A3_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A4_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A5_YX_0Z_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A6_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_A7_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_B0_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_B1_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_B2_0X_0N_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_B3_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_B4_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_B5_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_C0_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_C1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_C2_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_C3_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_C4_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_C5_00_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_D0_00_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_D1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);

void opcode_E0_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_E1_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_E2_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_E3_0X_LL_HH(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_E4_0X_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
void opcode_E5_YX_00_00(struct Chip16 * chip16, uint8_t a, uint8_t b, uint8_t c);
#endif