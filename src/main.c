#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ti/screen.h>
#include <ti/getcsc.h>

#define PROGRAM_MAGIC 0xFC
#define ROM_START 0xC0000
#define ROM_END 0x100000
#define RAM_START 0xD00000

typedef enum ti_progtype_t {
    PROGRAM = 0x05,
    EDIT_LOCKED_PRGM = 0x06,
    GROUP = 0x17,
    FLASH_APPLICATION = 0x24
} ti_progtype_t;

typedef struct {
	uint16_t meta_body_len;             // Len. of following header data + binary
	ti_progtype_t program_type;         // Type of program
	uint8_t type2;                      // Possibly used in PTT mode, usually 0x0
	uint8_t version;                    // Pgm version. Unknown purpose. <=0x6
	uint16_t program_offset;            // relative_block << 16 + offset
	uint8_t relative_block;             // Relative block in mem. to start offset
	uint8_t program_name_len;           // The length of the program name
	char* program_name;                 // The name of the proram
	uint16_t binary_length;             // The length of the raw binary blob
} ti_progmem_header_t;

void read_header(uint8_t* rom_data, uint16_t offset, ti_progmem_header_t* header) {
    header->meta_body_len = *(uint16_t*)&rom_data[offset + 1];
    header->program_type = rom_data[offset + 3];
    header->type2 = rom_data[offset + 4];
    header->version = rom_data[offset + 5];
    header->program_offset = *(uint16_t*)&rom_data[offset + 6];
    header->relative_block = rom_data[offset + 8];
    header->program_name_len = rom_data[offset + 9];
    
    header->program_name = (char*)malloc(header->program_name_len + 1);
    memcpy(header->program_name, &rom_data[offset + 10], header->program_name_len);
    header->program_name[header->program_name_len] = '\0';

    header->binary_length = *(uint16_t*)&rom_data[offset + 10 + header->program_name_len];
}

void search_rom_for_programs(uint8_t* rom_data, uint16_t start_offset, uint16_t end_offset, uint8_t relative_block) {
    for (uint16_t i = start_offset; i < end_offset; i++) {
        if (rom_data[i] == PROGRAM_MAGIC) {
            ti_progmem_header_t header;
            read_header(rom_data, i, &header);

            printf("\nFound program!  0x%X%04X\n", (relative_block + 0xC), i);
            printf("program_name:   %s\n", header.program_name);
            printf("meta_body_len:  0x%X\n", header.meta_body_len);
            printf("type2:          0x%02X\n", header.type2);
            printf("version:        0x%02X\n", header.version);
            printf("program_offset: 0x%X\n", header.program_offset);
            printf("relative_block: 0x%X\n", header.relative_block);
            //printf("program_name_len: %u\n", header.program_name_len);
            printf("binary_length:  0x%X\n", header.binary_length);

            i += 10 + header.program_name_len + header.binary_length;

            free(header.program_name);
            while (!os_GetCSC());
        }
    }
}

int main() 
{
    os_ClrHome();
    uint8_t* rom_data = (uint8_t*)ROM_START;

    // Iterate over ROM in 64k chunks because we can't use uint32_t.
    for (uint8_t relative_block = 0; relative_block < ((ROM_END - ROM_START) >> 16); relative_block++) {
        const uint16_t start_offset = 0;
        const uint16_t end_offset = 0xFFFF;

        search_rom_for_programs(rom_data + (relative_block << 16), start_offset, end_offset, relative_block);
    }

    return 0;
}