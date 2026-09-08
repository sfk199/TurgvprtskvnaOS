#include <stdbool.h>
#include <stddef.h>
#include <kernel/limine/limine_defs.h>
#include <hcf.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

static struct limine_framebuffer *framebuffer = NULL;

void framebuffer_set_pixel (uint64_t y, uint64_t x, bool c) {
    uint32_t *address = framebuffer->address;
    address[y * (framebuffer->pitch / 4) + x] = 0xFFFFFF * c;
}

uint64_t framebuffer_height () { return framebuffer->height; }
uint64_t framebuffer_width () { return framebuffer->width; }

void framebuffer_initialize () {
    // Ensure we got a framebuffer
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count == 0)
        hcf ();
    framebuffer = framebuffer_request.response->framebuffers[0];
}