#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <err.h>
#include "encoder.h"


int main(int argc, char* argv[])
{
    if (argc < 2)
        errx(1, "Argument missing: ./main <image_path>");

    SDL_Surface * surface = IMG_Load(argv[1]);
    SDL_Surface * new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    surface = new_surface;
    printf("width: %i, height: %i\n", surface->w, surface->h);

    FILE * f = fopen("test", "w");
    write_header(f, surface);
    SDL_LockSurface(surface);
    encode_image(f, surface->w, surface->h, (Uint32 *) surface->pixels, surface->format);
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    return 0;
}
