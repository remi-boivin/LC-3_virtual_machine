#include "../includes/registry.hpp"
#include <stdio.h>

Registry::Registry(){};

// uint16_t Registry::sign_extend(uint16_t x, int bit_count)
// {
//     if ((x >> (bit_count - 1)) & 1)
//     {
//         x |= (0xFFFF << bit_count);
//     }
//     return x;
// }

int Registry::sign_extend(int x, int n) {
  int m = 1 << (n - 1);
  return (x ^ m) - m;
}

void Registry::set(uint16_t position, uint16_t value)
{
    // printf("set value: %d\n", value);
    m_registry[position] = value;
}

uint16_t Registry::get(uint16_t address)
{
    return (m_registry[address]);
}

void Registry::update_flags(uint16_t r)
{
    if (m_registry[r] == 0)
    {
        // printf("Called\n");
        m_registry[R_COND] = FL_ZRO;
    }
    else if (m_registry[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        // printf("Called Neg\n");
        m_registry[R_COND] = FL_NEG;
    }
    else
    {
        // printf("Called Pos\n");
        m_registry[R_COND] = FL_POS;
    }
}