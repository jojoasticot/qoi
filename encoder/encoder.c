#include "encoder.h"

void write_header(FILE * file, SDL_Surface * surface)
{
    uint32_t buffer = 'q' << 24 |
                      'o' << 16 |
                      'i' << 8  |
                      'f' << 0;

    buffer = __builtin_bswap32(buffer);
    fwrite(&buffer, sizeof(buffer), 1, file);

    buffer = __builtin_bswap32(surface->w);
    fwrite(&buffer, sizeof(buffer), 1, file);

    buffer = __builtin_bswap32(surface->h);
    fwrite(&buffer, sizeof(buffer), 1, file);

    uint8_t channels = 3; // RGB in all cases
    fwrite(&channels, sizeof(channels), 1, file);

    uint8_t colorspace = 0; // sRGB in all cases
    fwrite(&colorspace, sizeof(colorspace), 1, file);
}

void write_rgb(FILE * file, Uint32 pixel)
{
    // TODO
}

void write_index(FILE * file, Uint32 pixel)
{
    // TODO
}

void write_diff(FILE * file, Uint32 pixel)
{
    // TODO
}

void write_luma(FILE * file, Uint32 pixel)
{
    // TODO
}

void encode_image(FILE * file, int width, int height, Uint32* pixels)
{
    Uint32 prev = 0x000000FF;
    Uint32 array[64];

    for (int i = 0; i < 64; i++)
        array[i] = 0;

    for (int i = 0; i < width * height; i++)
    {
        /* Uint32 cur = pixels[i]; */
    }

    uint32_t flag1 = 0;
    uint32_t flag2 = __builtin_bswap32(0x00000001);
    fwrite(&flag1, sizeof(flag1), 1, file);
    fwrite(&flag2, sizeof(flag2), 1, file);
}
