#include "../includes/image.hpp"

Image::Image(Memory *memory, const char *path) : m_memory(memory), m_path(path){};

uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

void Image::readFile(FILE *file)
{
    /* the origin tells us where in memory to place the image */
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    /* we know the maximum file size so we only need one fread */
    uint16_t max_read = MEMORY_MAX - origin;
    uint16_t *p = m_memory->get() + origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    /* swap to little endian */
    while (read-- > 0)
    {
        *p = swap16(*p);
        ++p;
    }
}

int Image::read()
{
    FILE *file = fopen(m_path, "rb");
    if (!file)
    {
        return 0;
    };
    readFile(file);
    fclose(file);
    return 1;
}