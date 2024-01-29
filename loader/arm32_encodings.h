#ifndef __ASM_ENCODINGS_H__
#define __ASM_ENCODINGS_H__

#include <stdint.h>

typedef enum registers {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    SP  = 13,
    LR  = 14,
    PC  = 15,
} registers;

typedef struct b_enc {
  union {
    struct __attribute__((__packed__)) {
      int imm24: 24;
      unsigned int l: 1; // Branch with Link flag
      unsigned int enc: 3; // 0b101
      unsigned int cond: 4; // 0b1110
    } bits;
    uint32_t raw;
  };
} b_enc;

inline uint32_t _B(uint8_t l, intptr_t IP, intptr_t DEST)
{
  b_enc instruction = {
    .bits = {
      .imm24 = ((DEST-IP) / 4) - 2,
      .l = l,
      .enc = 0b101,
      .cond = 0b1110
    }
  };

  return instruction.raw;
}

#define B_RANGE ((1 << 24) - 1)
#define B_OFFSET(x) ((intptr_t)(x) + 8) // branch jumps into addr - 8, so range is biased forward
#define B(IP, DEST)  (_B(0, (intptr_t)(IP), (intptr_t)(DEST)))
#define BL(IP, DEST) (_B(1, (intptr_t)(IP), (intptr_t)(DEST)))

typedef struct ldst_enc {
  union {
    struct __attribute__((__packed__)) {
      int imm12: 12;
      unsigned int rt: 4; // Source/Destination register
      unsigned int rn: 4; // Base register
      unsigned int bit20_1: 1; // 0: store to memory, 1: load from memory
      unsigned int w: 1; // 0: no write-back, 1: write address into base
      unsigned int b: 1; // 0: word, 1: byte
      unsigned int u: 1; // 0: subtract offset from base, 1: add to base
      unsigned int p: 1; // 0: post indexing, 1: pre indexing
      unsigned int enc: 3;
      unsigned int cond: 4;
    } bits;
    uint32_t raw;
  };
} ldst_enc;

// #define LDR_OFFS(RT, RN, IMM) (((ldst_enc){.bits = {.cond = 0b1110, .enc = 0b010, .p = 1, .u = (IMM >= 0), .b = 0, .w = 0, .bit20_1 = 1, .rn = RN, .rt = RT, .imm12 = (IMM >= 0) ? IMM : -IMM}}).raw)
static inline uint32_t LDR_OFFS(int RT, int RN, intptr_t IMM)
{
  ldst_enc instruction = {
    .bits = {
      .imm12 = (IMM >= 0) ? IMM : -IMM,
      .rt = RT,
      .rn = RN,
      .bit20_1 = 1,
      .w = 0,
      .b = 0,
      .u = (IMM >= 0),
      .p = 1,
      .enc = 0b010,
      .cond = 0b1110
    }
  };

  return instruction.raw;
}

#endif // __ASM_ENCODINGS_H__