#include "qoi.h"

void read_qoi_header(FILE *file, QOI_HEADER *header) {
	fread(header, QOI_HEADER_SIZE, 1, file);

    header->width = TRANS_INDIAN_32(header->width);
    header->height = TRANS_INDIAN_32(header->height);
}

PIXEL* decode_qoi_file(FILE *file, QOI_HEADER *header) {
	read_qoi_header(file, header);

	if (!(header->magic[0] == 'q' && header->magic[1] == 'o' && header->magic[2] == 'i' && header->magic[3] == 'f')) {
		return NULL;
	}

	uint64_t num_pixels = header->width * header->height;

	PIXEL* pixels = malloc(num_pixels * sizeof(PIXEL));
	PIXEL seen[64] = {0};
	PIXEL prev = {0, 0, 0, 255};

	for (uint64_t i = 0; i < num_pixels; i++) {
		uint8_t prefix = fgetc(file);

		if  (prefix == QOI_OP_RGB) {
			fread(&prev, 3, 1, file);
		} else if (prefix == QOI_OP_RGBA) {
			fread(&prev, 4, 1, file);
		}

		else {
			switch (prefix & 0xC0) {
				case QOI_OP_INDEX:
					prev = seen[prefix & 0x3F];
					break;

				case QOI_OP_DIFF:
				{
					int db = (prefix & 0x03) - 2;
					int dg = ((prefix & 0x0C) >> 2) - 2;
					int dr = ((prefix & 0x30) >> 4) - 2;

					prev.r = CHANNEL_WRAP(prev.r + dr); prev.g = CHANNEL_WRAP(prev.g + dg); prev.b = CHANNEL_WRAP(prev.b + db);
					break;

				}

				case QOI_OP_LUMA:
				{
					uint8_t dr_db = fgetc(file);
					int dg = (prefix & 0x3F) - 32;
					int dr = ((dr_db & 0xF0) >> 4) - 8 + dg;
					int db = (dr_db & 0x0F) - 8 + dg;

					prev.r = CHANNEL_WRAP(prev.r + dr); prev.g = CHANNEL_WRAP(prev.g + dg); prev.b = CHANNEL_WRAP(prev.b + db);
					break;
				}

				case QOI_OP_RUN:
					uint8_t repetitions = (prefix & 0x3F) + 1;

					for (int j = 0; j < repetitions - 1; j++) {
						pixels[i + j] = prev;
					}

					i += (repetitions - 1);
					break;
			}
		}

		pixels[i] = prev;
		seen[QOI_HASH(prev)] = prev;
	}

	uint64_t * end_marker = malloc(sizeof(uint64_t));

	fread(end_marker, 8, 1, file);

	if (*end_marker != (uint64_t)1<<56) {
		free_pixels(pixels);
		return NULL;
	}

	return pixels;
}

void free_pixels(PIXEL* pixels) {
	free(pixels);
}