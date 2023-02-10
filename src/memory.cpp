#include "../includes/memory.hpp"
#include <cstdio>

Memory::Memory(){};

uint16_t *Memory::get()
{
    return m_memory;
}

void Memory::write(uint16_t address, uint16_t val)
{
    m_memory[address] = val;
}

uint16_t Memory::read(uint16_t address)
{
    if (address == TermInfo::MR_KBSR)
    {
        if (m_termInfo->check_key())
        {
            m_memory[TermInfo::MR_KBSR] = (1 << 15);
            m_memory[TermInfo::MR_KBDR] = getchar();
        }
        else
        {
            m_memory[TermInfo::MR_KBSR] = 0;
        }
    }
    return m_memory[address];
}