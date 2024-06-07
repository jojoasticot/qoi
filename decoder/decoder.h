#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>

SDL_Surface * read_header(FILE * file);
Uint32 read_rgb(FILE * file, Uint32 prev, SDL_PixelFormat * format);
Uint32 read_rgba(FILE * file, SDL_PixelFormat * format);
Uint32 read_diff(Uint32 prev, SDL_PixelFormat * format, uint8_t diffs);
Uint32 read_luma(FILE * file, Uint32 prev, SDL_PixelFormat * format, uint8_t diffs);
int compute_index(Uint32 pixel, SDL_PixelFormat * format);
void read_qoi(FILE * file, SDL_Surface * surface);
