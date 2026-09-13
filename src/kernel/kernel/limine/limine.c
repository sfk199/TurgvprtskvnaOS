#include <kernel/hcf.h>
#include <kernel/limine/limine_defs.h>
#include <stddef.h>
#include <stdbool.h>

// https://github.com/Limine-Bootloader/limine-protocol/blob/trunk/PROTOCOL.md
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

void limine_initialize () {
    // Make sure limine actually understands current revision
    if (LIMINE_BASE_REVISION_SUPPORTED (limine_base_revision) == false)
        hcf ();
}