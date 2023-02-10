#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <stdint.h>

class Registry
{

public:
    enum
    {
        R_R0 = 0,
        R_R1,
        R_R2,
        R_R3,
        R_R4,
        R_R5,
        R_R6,
        R_R7,
        R_PC, /* program counter */
        R_COND,
        R_COUNT
    };
    enum
    {
        FL_POS = 1 << 0, /* P */
        FL_ZRO = 1 << 1, /* Z */
        FL_NEG = 1 << 2, /* N */
    };
    enum
    {
        OP_BR = 0, /* branch */
        OP_ADD,    /* add  */
        OP_LD,     /* load */
        OP_ST,     /* store */
        OP_JSR,    /* jump register */
        OP_AND,    /* bitwise and */
        OP_LDR,    /* load register */
        OP_STR,    /* store register */
        OP_RTI,    /* unused */
        OP_NOT,    /* bitwise not */
        OP_LDI,    /* load indirect */
        OP_STI,    /* store indirect */
        OP_JMP,    /* jump */
        OP_RES,    /* reserved (unused) */
        OP_LEA,    /* load effective address */
        OP_TRAP    /* execute trap */
    };

    enum
    {
        TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
        TRAP_OUT = 0x21,   /* output a character */
        TRAP_PUTS = 0x22,  /* output a word string */
        TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
        TRAP_PUTSP = 0x24, /* output a byte string */
        TRAP_HALT = 0x25   /* halt the program */
    };

    Registry();
    // uint16_t sign_extend(uint16_t x, int bit_count);
    uint16_t get(uint16_t address);

    int sign_extend(int x, int n);

    void set(uint16_t position, uint16_t value);

    void update_flags(uint16_t r);

private:
    uint16_t m_registry[R_COUNT];
};

#endif