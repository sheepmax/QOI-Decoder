#ifndef QOI_H

#define QOI_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

// Needed specifically due to alignment of structs
#define QOI_HEADER_SIZE 14

#define TRANS_INDIAN_32(x) ((((x) & 0xFF) << 24) | (((x) & 0xFF00) << 8) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF000000) >> 24))
#define QOI_HASH(x) ((((x).r * 3) + ((x).g * 5) + ((x).b * 7) + ((x).a * 11)) % 64)
#define CHANNEL_WRAP(x) (x + (((x) < 0) ? 256 : (((x) > 255) ? -256 : 0)))

#define QOI_OP_RGB   0xFE
#define QOI_OP_RGBA  0xFF
#define QOI_OP_INDEX 0x00
#define QOI_OP_DIFF  0x40
#define QOI_OP_LUMA  0x80
#define QOI_OP_RUN   0xC0

typedef struct qoi_header {
	char magic[4];		
	uint32_t width;
	uint32_t height;
	uint8_t channels;
	uint8_t colorspace;
} QOI_HEADER;

typedef struct pixel {
	uint8_t r, g, b, a;
} PIXEL;

void read_qoi_header(FILE *file, QOI_HEADER *header);

// Takes in a file and qoi header object, the header will be filled with relevant information.
// A pixel array is returned with the raw image data.
PIXEL* decode_qoi_file(FILE *file, QOI_HEADER *header);

void free_pixels(PIXEL* pixels);

#endif