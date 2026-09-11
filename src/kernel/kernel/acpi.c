#include <kernel/limine/limine_defs.h>
#include <kernel/memory/hhdm.h>
#include <kernel/log.h>
#include <kernel/hcf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Also read:
// https://wiki.osdev.org/ACPI

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 0
};

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;      // deprecated since version 2.0

    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__ ((packed)) xsdt_t;

#define ACPI_RSDP_XSDP_NOT_FOUND \
    do { kernel_printf ("[  ERROR ][  ACPI  ] RSDP/XSDP Not found, this is bad...\n"); hcf (); } while (false)

static void acpi_decode_xsdt (uint64_t xsdt_address); // Its implemented below

void acpi_initialize () {
    if (rsdp_request.response == NULL) ACPI_RSDP_XSDP_NOT_FOUND;

    xsdt_t* xsdp_table = rsdp_request.response->address;

    // Signature check
    if (memcmp (xsdp_table->signature, "RSD PTR ", 8)) { // Its (char)32 terminated, not (char)0...
        kernel_printf ("[  ERROR ][  ACPI  ] RSDP/XSDP Signature doesn't match, expected: \"RSD PTR \", got: \"");
        for (size_t i = 0; i < 8; i++) kernel_putchar (xsdp_table->signature[i]);
        kernel_printf ("\"\n");
        ACPI_RSDP_XSDP_NOT_FOUND;
    }
    
    // Checksum check
    uint8_t checksum = 0;
    for (size_t i = 0; i < 20; i++)
        checksum += ((uint8_t*)xsdp_table)[i];
    if (checksum != 0) {
        kernel_printf ("[  ERROR ][  ACPI  ] RSDP/XSDP Checksum failed, sum is: %u\n", checksum);
        ACPI_RSDP_XSDP_NOT_FOUND;
    }

    // Check revision
    uint8_t revision = xsdp_table->revision;
    if (revision == 0) {
        kernel_printf ("[  WARN  ][  ACPI  ] RSDP/XSDP table revision is zero, using limine provided...\n");
        revision = rsdp_request.response->revision;
    }

    // Print some data...
    kernel_printf ("[  INFO  ][  ACPI  ] ACPI Revision %u, OEM=\"", revision);
    for (size_t i = 0; i < 6; i++) kernel_putchar (xsdp_table->oemid[i]);
    kernel_printf ("\"\n");

    if (revision == 0)
        kernel_printf ("[  WARN  ][  ACPI  ] ACPI Revision is 0, your firmware is buggy, trying to use XSDT anyways...\n");
    else if (revision == 1)
        kernel_printf ("[  WARN  ][  ACPI  ] ACPI Revision is 1, your firmware is too old, trying to use XSDT anyways...\n");

    checksum = 0;
    for (int i = 0; i < xsdp_table->length; i++)
        checksum += ((uint8_t*)xsdp_table)[i];
    if (checksum != 0 || xsdp_table->xsdt_address == 0) {
        if (checksum != 0)
            kernel_printf ("[  ERROR ][  ACPI  ] XSDP Checksum failed, sum is: %u\n", checksum);
        else
            kernel_printf ("[  ERROR ][  ACPI  ] XSDT address not found\n");
        kernel_printf ("[  ERROR ][  ACPI  ] RSDT not yet supported, XSDT not found, your firmware is too old\n");
        kernel_printf ("[  INFO  ][  ACPI  ] If you are using QEMU, try using OVMF firmware (or `make run` command)\n");
        ACPI_RSDP_XSDP_NOT_FOUND;
    }

    acpi_decode_xsdt (xsdp_table->xsdt_address + (int64_t)get_hhdm_offset());

    kernel_printf ("[  INFO  ][  ACPI  ] ACPI Initialized!\n");
}

#undef ACPI_RSDP_XSDP_NOT_FOUND

// We have pointer to XSDT structure, now lets actually decode stuff
// Decoding logic is implemented below

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

    // And after this is uint64_t pointers to other SDTs. (.length - sizeof (acpi_sdt_header))
} __attribute__ ((packed)) acpi_sdt_header;

static bool acpi_check_sdt_header_checksum (acpi_sdt_header* header) {
    uint8_t sum = 0;
    for (int i = 0; i < header->length; i++)
        sum += ((uint8_t*)header)[i];
    return sum == 0;
}

static void acpi_decode_xsdt (uint64_t xsdt_address) {
    acpi_sdt_header* xsdt = (acpi_sdt_header*)xsdt_address;

    if (!acpi_check_sdt_header_checksum (xsdt)) {
        kernel_printf ("[  ERROR ][  ACPI  ] XSDT table checksum failed\n");
        hcf ();
    }
}