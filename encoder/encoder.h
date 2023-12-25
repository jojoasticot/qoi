#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void write_header(FILE * file, SDL_Surface * surface);
void write_rgb(FILE * file, uint8_t r, uint8_t g, uint8_t b);
void write_rgba(FILE * file, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void write_index(FILE * file, uint8_t index);
void write_diff(FILE * file, uint8_t dr, uint8_t dg, uint8_t db);
void write_luma(FILE * file, uint8_t dg, uint8_t dr_dg, uint8_t db_dg);
void write_run(FILE * file, uint8_t length);
void encode_image(FILE * file, int width, int height, uint32_t* pixels, SDL_PixelFormat * format);
