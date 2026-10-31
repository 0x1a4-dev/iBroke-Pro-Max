#pragma once
#include <stdint.h>
typedef struct { uint32_t code,offset; uint8_t w,h; int8_t x,y; uint8_t advance; } Glyph;
#define GLYPHS_PER_FONT 118
#define ICON_SIZE 32
extern const Glyph glyphs[];
extern const uint8_t font_pixels[],app_alpha[];
extern const uint16_t wallpaper0[],wallpaper1[],app_icons[],portrait_pixels[];
