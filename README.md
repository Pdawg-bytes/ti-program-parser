# TI Program Parser
This is a simple C program that searches your calculator's ROM from 0xC0000 to a defined end region for programs inside of the ROM. Each program follows a structure that I'll provide below.

## Demo
![](https://raw.githubusercontent.com/Pdawg-bytes/ti-program-parser/main/screen_record/screen.gif)

## Program structure in Archive
```c
// Same as 8xp->meta_data->file_type
typedef enum {
    REAL = 0x00,
    REAL_LIST = 0x01,
    MATRIX = 0x02,
    EQUATON = 0x03,
    STRING = 0x04,
    PROGRAM = 0x05,
    PROTECTED_PROGRAM = 0x06,
    PICTURE = 0x07,
    GRAPH_DB = 0x08,
    SMART_EQUATION = 0x0B,
    COMPLEX = 0x0C,
    COMPLEX_LIST = 0x0D,
    WINDOW_SETTINGS = 0x0F,
    RECALL_WINDOW = 0x10,
    TABLE_RANGE = 0x11,
    SCREEN_IMAGE = 0x12,
    BACKUP = 0x13,
    APP = 0x14,
    APP_VAR = 0x15,
    PYTHON_APP_VAR = 0x15,
    TEMP_ITEM = 0x16,
    GROUP_OBJECT = 0x17,
    REAL_FRACTION = 0x18,
    MIXED_FRACTION = 0x19,
    IMAGE = 0x1A,
    EXACT_COMPLEX_FRACTION = 0x1B,
    EXACT_REAL_RADICAL = 0x1C,
    EXACT_COMPLEX_RADICAL = 0x1D,
    EXACT_COMPLEX_PI = 0x1E,
    EXACT_COMPLEX_PI_FRAC = 0x1F,
    EXACT_REAL_PI = 0x20,
    EXACT_REAL_PI_FRAC = 0x21,
    OPERATING_SYSTEM = 0x23,
    FLASH_APP = 0x24,
    CERTIFICATE = 0x25,
    APP_ID_LIST = 0x26,
    CERTIFICATE_MEMORY = 0x27,
    CLOCK = 0x29,
    FLASH_LICENSE = 0x3E,
} ti_progtype_t;


/*
Behavior of each value:

- program_magic remains constant, and is always the first byte in the header.

- program_type is the same found in the 8xp metadata as mentioned above.

- type2 is possibly used in OS 5.1/5.2+ (according to Adriweb) in PTT mode, but is otherwise ignored. TIs docs mention it as an unused flag byte.

- version is the version found in the 8xp metadata field, but the calculator will not accept the file if version is over 0x06.

- program_offset is the offset from the (relative block << 16) at which the beginning of the header is.

- relative_block is the relative block of addresses in which the program is stored. it's used to calculate the offset. 0xD with a program_offset of 0 means that the program starts at 0xD0000. if an entire program does not fit in one block, it is shifted up to the next one.

- progname_length is the length (in bytes) of the program name.

- program_name is the name of the program itself. the name is a variable length, but the 8XP format itself limits the size to a fixed 8 chars, and the 8xp header is a fixed size that fills missing chars with 0s.

- binary_length is the length (in bytes) of the raw binary blob.
*/
typedef struct {
	const uint8_t program_magic = 0xFC; // Beginning of every program
	uint16_t meta_body_length;          // Len. of following header data + binary
	ti_progtype_t program_type;         // Type of program
	uint8_t type2;                      // Possibly used in PTT mode, usually 0x0
	uint8_t version;                    // Pgm version. Unknown purpose. <=0x6
	uint16_t program_offset;            // relative_block << 16 + offset
	uint8_t relative_block;             // Relative block in mem. to start offset
	uint8_t progname_length;            // The length of the program name
	char* program_name;                 // The name of the proram
	uint16_t binary_length;             // The length of the raw binary blob
} ti_progmem_header_t;
```
