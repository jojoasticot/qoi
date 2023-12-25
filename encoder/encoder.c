#include "encoder.h"

#define DEBUG   1

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

    uint8_t channels = 4; // always rgba
    fwrite(&channels, sizeof(channels), 1, file);

    uint8_t colorspace = 0; // sRGB in all cases
    fwrite(&colorspace, sizeof(colorspace), 1, file);
}

void write_rgb(FILE * file, uint8_t r, uint8_t g, uint8_t b)
{
    if (DEBUG)
        printf("write_rgb, %u, %u, %u\n", r, g, b);

    uint8_t flag = 0xFE;
    fwrite(&flag, sizeof(flag), 1, file);
    fwrite(&r, sizeof(r), 1, file);
    fwrite(&g, sizeof(g), 1, file);
    fwrite(&b, sizeof(b), 1, file);
}

void write_rgba(FILE * file, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (DEBUG)
        printf("write_rgba, %u, %u, %u, %u\n", r, g, b, a);

    uint8_t flag = 0xFF;
    fwrite(&flag, sizeof(flag), 1, file);
    fwrite(&r, sizeof(r), 1, file);
    fwrite(&g, sizeof(g), 1, file);
    fwrite(&b, sizeof(b), 1, file);
    fwrite(&a, sizeof(a), 1, file);
}

void write_index(FILE * file, uint8_t index)
{
    if (DEBUG)
        printf("write_index, %u\n", index);

    fwrite(&index, sizeof(index), 1, file); // index has a max value of 63, so its two msb are 00 
}

void write_diff(FILE * file, uint8_t dr, uint8_t dg, uint8_t db)
{
    if (DEBUG)
        printf("write_diff, %u, %u, %u\n", dr, dg, db);

    // the differences have already been bias-processed
    uint8_t flag = 0x01;
    uint8_t qoi_diff = flag << 6 | dr << 4 | dg << 2 | db;
    fwrite(&qoi_diff, sizeof(qoi_diff), 1, file);
}

void write_luma(FILE * file, uint8_t dg, uint8_t dr_dg, uint8_t db_dg)
{
    if (DEBUG)
        printf("write_luma, %u, %u, %u\n", dg, dr_dg, db_dg);

    // the differences have already been bias-processed
    uint8_t flag = 0x02;
    uint8_t luma1 = flag << 6 | dg;
    uint8_t luma2 = dr_dg << 4 | db_dg;

    fwrite(&luma1, sizeof(luma1), 1, file);
    fwrite(&luma2, sizeof(luma2), 1, file);
}

void write_run(FILE * file, uint8_t length)
{
    if (DEBUG)
        printf("write_run, %u\n", length);

    uint8_t flag = 0x03;
    uint8_t qoi_run = flag << 6 | (length - 1); // bias of -1

    fwrite(&qoi_run, sizeof(qoi_run), 1, file);
}

void encode_image(FILE * file, int width, int height, Uint32 * pixels, SDL_PixelFormat * format)
{
    Uint32 prev = 0x000000FF;
    Uint32 array[64];

    for (int i = 0; i < 64; i++)
        array[i] = 0;

    uint8_t run = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            Uint8 p_r, p_g, p_b, p_a;
            SDL_GetRGBA(prev, format, &p_r, &p_g, &p_b, &p_a);
            if (DEBUG)
                printf("x: %i, y: %i; ", x, y);
            if (DEBUG)
                printf("prev: %u, %u, %u, %u; cur = %u, %u, %u, %u\n", p_r, p_g, p_b, p_a, r, g, b, a);
            uint8_t index = (r * 3 + g * 5 + b * 7 + a * 11) % 64;

            if (prev == pixel)
            {
                array[index] = pixel;
                run++;
                if (run == 62)
                {
                    write_run(file, run);
                    run = 0;
                }
                continue;
            }

            if (run > 0)
            {
                write_run(file, run);
                run = 0;
            }

            prev = pixel;
            
            if (array[index] == pixel)
            {
                write_index(file, index);
                continue;
            }

            array[index] = pixel;

            if (a != p_a) // alpha value has to remain unchanged for the next op_qois
            {
                write_rgba(file, r, g, b, a);
                continue;
            }

            uint8_t dr = r - p_r + 2; // all diffs are biased by 2
            uint8_t dg = g - p_g + 2;
            uint8_t db = b - p_b + 2;

            if (dr < 4 && dg < 4 && db < 4)
            {
                write_diff(file, dr, dg, db);
                continue;
            }

            uint8_t dr_dg = dr - dg + 8;
            uint8_t db_dg = db - dg + 8;
            dr += 30; // bias of 32 for dr
            
            if (dr < 64 && dr_dg < 16 && db_dg < 16)
            {
                write_luma(file, dr, dr_dg, db_dg);
                continue;
            }

            write_rgb(file, r, g, b);
        }
    }

    if (run > 0)
        write_run(file, run);

    uint32_t flag1 = 0;
    uint32_t flag2 = __builtin_bswap32(0x00000001);
    fwrite(&flag1, sizeof(flag1), 1, file);
    fwrite(&flag2, sizeof(flag2), 1, file);
}
