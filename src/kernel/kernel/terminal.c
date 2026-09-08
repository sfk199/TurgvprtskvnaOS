#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/framebuffer.h>
#include <hcf.h>
#include <kernel/font.h>

static const uint64_t font_table[] = GLYPH_FONT;
static uint32_t cursor_y = 0, cursor_x = 0; // Only checked for validity when drawing

#define GLYPH_WIDTH     5
#define GLYPH_HEIGHT    12
#define GLYPH_STRIDE_X  GLYPH_WIDTH  + 1
#define GLYPH_STRIDE_Y  GLYPH_HEIGHT + 2

void terminal_putchar (char symbol) {
    if (cursor_x > framebuffer_width() || symbol == '\n')
        cursor_x = 0, cursor_y += GLYPH_STRIDE_Y;
    if (cursor_y > framebuffer_height())
        cursor_x = 0, cursor_y = 0;

    if (symbol < 32 || symbol > 127)
        return;

    const uint32_t top_glyph = font_table[symbol - 32] >> 32;
    const uint32_t bottom_glyph = font_table[symbol - 32];
    for (size_t y = 0; y < GLYPH_HEIGHT / 2; y++)
        for (size_t x = 0; x < GLYPH_WIDTH; x++)
            framebuffer_set_pixel (cursor_y + y, cursor_x + x, top_glyph & (1 << (5 * y + x)));
    for (size_t y = 0; y < GLYPH_HEIGHT / 2; y++)
        for (size_t x = 0; x < GLYPH_WIDTH; x++)
            framebuffer_set_pixel (cursor_y + y + GLYPH_HEIGHT / 2, cursor_x + x, bottom_glyph & (1 << (5 * y + x)));
    cursor_x += GLYPH_STRIDE_X;
}

void terminal_writestring (const char* str) {
    for (size_t i = 0; str[i]; i++) terminal_putchar (str[i]);
}

void terminal_write (const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar (data[i]);
}

void terminal_initialize () {
    terminal_writestring ("[  INFO  ] Terminal init!\n");
}