#ifndef KERNEL_DEVICES_DISPLAY_FRAMEBUFFER
#define KERNEL_DEVICES_DISPLAY_FRAMEBUFFER

#include <stdbool.h>
#include <stdint.h>
void framebuffer_set_pixel (uint64_t y, uint64_t x, bool c);
void framebuffer_set_pixel_rgb (uint64_t y, uint64_t x, uint32_t rgb);
uint64_t framebuffer_height ();
uint64_t framebuffer_width ();
bool framebuffer_initialized ();
void framebuffer_initialize ();

#endif