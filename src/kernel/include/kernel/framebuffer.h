#include <stdbool.h>
#include <stdint.h>

void framebuffer_set_pixel (uint64_t y, uint64_t x, bool c);
uint64_t framebuffer_height ();
uint64_t framebuffer_width ();
void framebuffer_initialize ();