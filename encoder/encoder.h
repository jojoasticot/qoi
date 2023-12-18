#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void write_header(FILE * file, SDL_Surface * surface);
void write_rgb(FILE * file, Uint32 pixel);
void write_index(FILE * file, Uint32 pixel);
void write_diff(FILE * file, Uint32 pixel);
void write_luma(FILE * file, Uint32 pixel);
void encode_image(FILE * file, int width, int height, Uint32* pixels);
