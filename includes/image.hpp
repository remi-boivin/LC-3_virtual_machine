#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <stdio.h>
#include "../includes/memory.hpp"

class Image
{

public:
    Image(Memory *memory, const char *path);
    void readFile(FILE *file);
    int read();

private:
    Memory *m_memory;
    const char *m_path;
};

#endif // IMAGE_HPP