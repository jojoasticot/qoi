#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <err.h>
#include "decoder.h"


int main(int argc, char* argv[])
{
    if (argc < 2)
        errx(1, "Argument missing: ./main <qoi_file>");

    char * file = argv[1];
    char * filename;
    char * point = strchr(file, '.');
    if (point)
    {
        filename = malloc(point - file + 1);
        strncpy(filename, file, point - file);
        filename[point - file] = '\0';
    }
    else
        filename = file;
    FILE * f = fopen(file, "r");

    SDL_Surface * surface = read_header(f);
    SDL_Surface * new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    surface = new_surface;

    SDL_LockSurface(surface);
    read_qoi(f, surface);
    SDL_UnlockSurface(surface);

    char *output_filename = malloc(strlen(filename) + 5); // 4 characters for ".png" + 1 character for null terminator
    strcpy(output_filename, filename);
    strcat(output_filename, ".png");

    IMG_SavePNG(surface, output_filename);

    free(output_filename);

    return 0;
}
