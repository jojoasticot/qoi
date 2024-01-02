#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <err.h>
#include "decoder.h"


int main(int argc, char* argv[])
{
    if (argc < 2)
        errx(1, "Argument missing: ./main <qoi_file>");

    char * filename = argv[1];
    FILE * f = fopen(filename, "r");

    SDL_Surface * surface = read_header(f);

    SDL_LockSurface(surface);
    read_qoi(f, surface);
    SDL_UnlockSurface(surface);

    IMG_SavePNG(surface, "test");

    return 0;
}
