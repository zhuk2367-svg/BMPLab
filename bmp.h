#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdbool.h>
#include "image.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPFileHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_ppm;
    int32_t y_ppm;
    uint32_t colors_used;
    uint32_t colors_important;
} BMPInfoHeader;

typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    Image* image;
} BMPImage;
#pragma pack(pop)

BMPImage* bmp_load(const char* filename);
bool bmp_save(BMPImage* bmp, const char* filename);
void bmp_free(BMPImage* bmp);
int calculate_row_padding(int width);

#endif