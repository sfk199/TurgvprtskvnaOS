#include <kernel/memory/hhdm.h>
#include <kernel/devices/display/framebuffer.h>
#include <kernel/log.h>
#include <stdint.h>
#include <string.h>

// Also read:
// https://wiki.osdev.org/BGRT
// https://en.wikipedia.org/wiki/BMP_file_format

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;

    // bgrt specific fields
    uint16_t version_id;
    uint8_t status;
    uint8_t image_type;
    uint64_t image_address;
    uint32_t image_x_offset;
    uint32_t image_y_offset;
} __attribute__ ((packed)) bgrt_t;

typedef struct {
    char signature[2];
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t file_offset_to_pixelarray;

    uint32_t dib_header_size;
    uint32_t image_width;
    uint32_t image_height;
    uint16_t planes;
    uint16_t bpp;
    // We dont care about rest
} __attribute__ ((packed)) bmp_header_t;

void acpi_decode_bgrt (uint64_t bgrt_address) {
    bgrt_t* bgrt = (bgrt_t*)bgrt_address;

    if (bgrt->version_id != 1) {
        kernel_printf ("[  WARN  ][  ACPI  ][  BGRT  ] Unimplemented BGRT version\n");
        return;
    }

    if (bgrt->image_type != 0) {
        kernel_printf ("[  WARN  ][  ACPI  ][  BGRT  ] Unimplemented BGRT image type\n");
        return;
    }

    if ((bgrt->status & 0b00000110) != 0) {
        kernel_printf ("[  WARN  ][  ACPI  ][  BGRT  ] Unimplemented BGRT orientation offset\n");
        return;
    }

    bmp_header_t* bmp_header = (bmp_header_t*)(bgrt->image_address + get_hhdm_offset());
    if (memcmp (bmp_header->signature, "BM", 2)) {
        kernel_printf ("[  WARN  ][  ACPI  ][  BGRT  ] BMP signature doesn't match\n");
        return;
    }

    if (bmp_header->bpp != 24 && bmp_header->bpp != 32) {
        kernel_printf ("[  WARN  ][  ACPI  ][  BGRT  ] BMP bpp is not supported\n");
        return;
    }

    kernel_printf ("[  INFO  ][  ACPI  ][  BGRT  ] Decoding %u byte image (%ux%u at %ubpp)\n", bmp_header->file_size, bmp_header->image_width, bmp_header->image_height, bmp_header->bpp);

    uint8_t* pointer = (uint8_t*)(bmp_header->file_offset_to_pixelarray + bgrt->image_address + get_hhdm_offset());
    for (size_t y_t = 0; y_t < bmp_header->image_height; y_t++) {
        size_t y = bmp_header->image_height - 1 - y_t;
        for (size_t x = 0; x < bmp_header->image_width; x++) {
            if (bmp_header->bpp != 24) // there is extra reserved byte at the begining (alpha channel)
                pointer++;
            uint32_t rgb = (*(pointer + 2) << 16) + (*(pointer + 1) << 8) + *(pointer + 0);
            pointer += 3;

            framebuffer_set_pixel_rgb (y + bgrt->image_y_offset, x + bgrt->image_x_offset, rgb);
        }

        // Rows are aligned to 4byte
        if (bmp_header->bpp == 24)
            pointer += (4 - ((bmp_header->image_width * 3) % 4)) % 4; // help
    }

}