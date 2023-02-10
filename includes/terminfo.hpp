#ifndef TERMINFO_HPP
#define TERMINFO_HPP

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

class TermInfo
{

public:
    enum KeyboardSignal
    {
        MR_KBSR = 0xFE00, /* keyboard status */
        MR_KBDR = 0xFE02  /* keyboard data */
    };

    TermInfo();
    uint16_t check_key();
};

#endif // TERMINFO_HPP