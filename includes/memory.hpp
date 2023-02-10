#ifndef MY_MEMORY_HPP
#define MY_MEMORY_HPP

#define MEMORY_MAX (1 << 16)

#include <stdint.h>
#include "../includes/terminfo.hpp"

class Memory
{

public:
    Memory();
    uint16_t *get();
    uint16_t read(uint16_t address);

    void set_memory(uint16_t memory);
    void write(uint16_t address, uint16_t val);

private:
    uint16_t m_memory[MEMORY_MAX]; /* 65536 locations */
    TermInfo *m_termInfo = new TermInfo();
};

#endif // MY_MEMORY_HPP