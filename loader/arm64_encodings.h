#ifndef __ASM_ENCODINGS_H__
#define __ASM_ENCODINGS_H__

#include <stdint.h>

typedef enum registers {
    // 64-bit general-purpose registers
    X0 = 0,   R0 = X0,
    X1 = 1,   R1 = X1,
    X2 = 2,   R2 = X2,
    X3 = 3,   R3 = X3,
    X4 = 4,   R4 = X4,
    X5 = 5,   R5 = X5,
    X6 = 6,   R6 = X6,
    X7 = 7,   R7 = X7,
    X8 = 8,   R8 = X8,
    X9 = 9,   R9 = X9,
    X10 = 10, R10 = X10,
    X11 = 11, R11 = X11,
    X12 = 12, R12 = X12,
    X13 = 13, R13 = X13,
    X14 = 14, R14 = X14,
    X15 = 15, R15 = X15,
    X16 = 16, R16 = X16,
    X17 = 17, R17 = X17,
    X18 = 18, R18 = X18,
    X19 = 19, R19 = X19,
    X20 = 20, R20 = X20,
    X21 = 21, R21 = X21,
    X22 = 22, R22 = X22,
    X23 = 23, R23 = X23,
    X24 = 24, R24 = X24,
    X25 = 25, R25 = X25,
    X26 = 26, R26 = X26,
    X27 = 27, R27 = X27,
    X28 = 28, R28 = X28,
    X29 = 29, R29 = X29,
    X30 = 30, R30 = X30,
    X31 = 31, R31 = X31,

    // Aliases
    W0 = X0, W1 = X1, W2 = X2, W3 = X3, W4 = X4, W5 = X5, W6 = X6, W7 = X7,
    W8 = X8, W9 = X9, W10 = X10, W11 = X11, W12 = X12, W13 = X13, W14 = X14, W15 = X15,
    W16 = X16, W17 = X17, W18 = X18, W19 = X19, W20 = X20, W21 = X21, W22 = X22, W23 = X23,
    W24 = X24, W25 = X25, W26 = X26, W27 = X27, W28 = X28, W29 = X29, W30 = X30, W31 = X31,
    SP = X31
} registers;

typedef struct ldr_lit_enc {
  union {
    struct __attribute__((__packed__)) {
      unsigned int rt: 5;
      unsigned int imm19: 19;
      unsigned int opc: 8;
    } bits;
    uint32_t raw;
  };
} ldr_lit_enc;

static inline uint32_t LDR_LIT_QWORD(uint32_t RT, uint32_t LIT)
{
  ldr_lit_enc instruction { RT, LIT / 4, 0b00011000 | 0b01000000 };
  return instruction.raw;
}

static inline uint32_t LDR_LIT_DWORD(uint32_t RT, int32_t LIT)
{
  int flag = 0;
  if (LIT < 0)
    flag |= 0b10000000;

  ldr_lit_enc instruction { RT, (uint32_t)(LIT / 4), (uint32_t)(0b00011000 | flag) };
  return instruction.raw;
}

typedef struct b_enc {
  union {
    struct __attribute__((__packed__)) {
      int imm26: 26;
      unsigned int enc: 6; // 0b1110
    } bits;
    uint32_t raw;
  };
} b_enc;

typedef struct br_enc {
  union {
    struct __attribute__((__packed__)) {
      unsigned int rm: 5;
      unsigned int rn: 5;
      unsigned int m: 1;
      unsigned int a: 1;
      unsigned int op: 11;
      unsigned int z: 2;
      unsigned int enc: 7;
    } bits;
    uint32_t raw;
  };
} br_enc;

static inline uint32_t BRANCH_ENC(uint32_t PC, intptr_t DEST, uint32_t enc)
{
  b_enc instruction { (int)((DEST - PC) / 4), enc };
  return instruction.raw;
}

static inline uint32_t BR(uint32_t Rn)
{
  br_enc instruction { 0, Rn, 0, 0, 0b00111110000, 0, 0b1101011 };
  return instruction.raw;
}

#define B_RANGE ((1 << 26) - 1)
#define B_OFFSET(x) (x + 8) // branch jumps into addr - 8, so range is biased forward
#define B(PC, DEST)  (BRANCH_ENC(PC, DEST, 0b000101))
#define BL(PC, DEST) (BRANCH_ENC(PC, DEST, 0b100101))

#endif // __ASM_ENCODINGS_H__