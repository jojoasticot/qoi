#include "decoder.h"

#define DEBUG   0

SDL_Surface * read_header(FILE * file)
{
    uint32_t flag;
    fread(&flag, sizeof(flag), 1, file);

    if ((flag & 0xff000000) >> 6 * 4 != 'f' ||
        (flag & 0x00ff0000) >> 4 * 4 != 'i' ||
        (flag & 0x0000ff00) >> 2 * 4 != 'o' ||
        (flag & 0x000000ff) >> 0 * 4 != 'q')
        errx(1, "Error: Not a qoi file.");

    uint32_t width;
    fread(&width, sizeof(width), 1, file);
    width = __builtin_bswap32(width);

    uint32_t height;
    fread(&height, sizeof(height), 1, file);
    height = __builtin_bswap32(height);

    if (DEBUG)
        printf("%i, %i\n", width, height);

    uint8_t channels;
    fread(&channels, sizeof(channels), 1, file);

    uint8_t colorspace;
    fread(&colorspace, sizeof(colorspace), 1, file);

    SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    return surface;
}

Uint32 read_rgb(FILE * file, Uint32 prev, SDL_PixelFormat * format)
{
    uint8_t r, g, b, a;
    fread(&r, sizeof(r), 1, file);
    fread(&g, sizeof(g), 1, file);
    fread(&b, sizeof(b), 1, file);
    a = prev & 0x000000FF;

    if (DEBUG)
        printf("read_rgb, %u, %u, %u\n", r, g, b);

    return SDL_MapRGBA(format, r, g, b, a);
}

Uint32 read_rgba(FILE * file, SDL_PixelFormat * format)
{
    uint8_t r, g, b, a;
    fread(&r, sizeof(r), 1, file);
    fread(&g, sizeof(g), 1, file);
    fread(&b, sizeof(b), 1, file);
    fread(&a, sizeof(a), 1, file);

    if (DEBUG)
        printf("read_rgba, %u, %u, %u, %u\n", r, g, b, a);

    return SDL_MapRGBA(format, r, g, b, a);
}

Uint32 read_diff(Uint32 prev, SDL_PixelFormat * format, uint8_t diffs)
{
    uint8_t p_r, p_g, p_b, p_a;
    SDL_GetRGBA(prev, format, &p_r, &p_g, &p_b, &p_a);

    uint8_t r = ((diffs & 0x30) >> 4) + p_r - 2;
    uint8_t g = ((diffs & 0x0c) >> 2) + p_g - 2;
    uint8_t b = ((diffs & 0x03) >> 0) + p_b - 2;

    if (DEBUG)
        printf("read_rgba, %u, %u, %u\n", r, g, b);

    return SDL_MapRGBA(format, r, g, b, p_a);
}

Uint32 read_luma(FILE * file, Uint32 prev, SDL_PixelFormat * format, uint8_t diffs)
{
    uint8_t p_r, p_g, p_b, p_a;
    SDL_GetRGBA(prev, format, &p_r, &p_g, &p_b, &p_a);

    uint8_t u_d_g = diffs & 0x3f;
    int d_g = u_d_g - 32;
    uint8_t flag2;
    fread(&flag2, sizeof(flag2), 1, file);

    uint8_t u_d_r = ((flag2 & 0xf0) >> 4) + d_g;
    uint8_t u_d_b = ((flag2 & 0x0f) >> 0) + d_g;
    int d_r = u_d_r - 8;
    int d_b = u_d_b - 8;

    uint8_t r = d_r + p_r;
    uint8_t g = d_g + p_g;
    uint8_t b = d_b + p_b;

    if (DEBUG)
        printf("read_rgba, %u, %u, %u\n", r, g, b);

    return SDL_MapRGBA(format, r, g, b, p_a);
}

int compute_index(Uint32 pixel, SDL_PixelFormat * format)
{
    uint8_t r, g, b, a;
    SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
    return (r * 3 + g * 5 + b * 7 + a * 11) % 64;
}

void read_qoi(FILE * file, SDL_Surface * surface)
{
    Uint32 * pixels = surface->pixels;
    SDL_PixelFormat * format = surface->format;
    int w = surface->w;
    int h = surface->h;
    Uint32 prev = 0x000000FF;
    Uint32 array[64];

    for (int i = 0; i < 64; i++)
        array[i] = 0;

    int i = 0;
    uint8_t flag;

    while (i < w * h)
    {
        fread(&flag, sizeof(flag), 1, file);
        if (DEBUG)
            printf("flag: %2x\n", flag);

        if (flag == 0xFE) // QOI_OP_RGB
        {
            prev = read_rgb(file, prev, format);
            pixels[i] = prev;
            array[compute_index(prev, format)] = prev;
            i++;
            continue;
        }

        if (flag == 0xFF) //QOI_OP_RGBA
        {
            prev = read_rgba(file, format);
            pixels[i] = prev;
            array[compute_index(prev, format)] = prev;
            i++;
            continue;
        }

        uint8_t flag2 = flag & 0xc0;

        if (flag2 == 0) // QOI_OP_INDEX
        {
            // flag == index
            prev = array[flag];
            pixels[i] = prev;
            i++;
            continue;
        }

        if (flag2 == 0x40) // QOI_OP_DIFF
        {
            prev = read_diff(prev, format, flag);
            pixels[i] = prev;
            array[compute_index(prev, format)] = prev;
            i++;
            continue;
        }

        if (flag2 == 0x80) // QOI_OP_LUMA
        {
            prev = read_luma(file, prev, format, flag);
            pixels[i] = prev;
            array[compute_index(prev, format)] = prev;
            i++;
            continue;
        }

        if (flag2 == 0xc0) // QOI_OP_RUN
        {
            uint8_t run = flag & 0x3f;

            for (uint8_t j = 0; j < run + 1; j++)
            {
                pixels[i] = prev;
                i++;
            }

            array[compute_index(prev, format)] = prev;
            continue;
        }

        errx(1, "Error: Invalid qoi file");
    }

    uint32_t flag1;
    uint32_t flag2;
    fread(&flag1, sizeof(flag1), 1, file);
    fread(&flag2, sizeof(flag2), 1, file);

    if (flag1 != 0 && flag2 != 1)
        errx(1, "Error: wrong end flag, %u %u", flag1, flag2);
}
