#include <kernel/limine/limine_defs.h>
#include <kernel/hcf.h>
#include <stdint.h>
#include <stddef.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};
static uint64_t hhdm_offset = 0;
uint64_t get_hhdm_offset () { return hhdm_offset; };

void hhdm_initialize () {
    if (hhdm_request.response == NULL)
        hcf ();
    hhdm_offset = (uint64_t)hhdm_request.response->address;
}