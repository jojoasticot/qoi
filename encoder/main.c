#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "encoder.h"


int main(int argc, char* argv[])
{
    SDL_Surface * surface = IMG_Load(argv[1]);
    printf("width: %i, height: %i\n", surface->w, surface->h);

    FILE * f = fopen("test", "w");
    write_header(f, surface);
    encode_image(f, surface->w, surface->h, (Uint32 *) surface->pixels);

    return 0;
}
