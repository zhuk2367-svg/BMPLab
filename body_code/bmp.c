#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BMPImage* bmp_load(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }

    BMPImage* bmp = (BMPImage*)malloc(sizeof(BMPImage));
    if (!bmp) {
        fclose(file);
        return NULL;
    }

    if (fread(&bmp->file_header, sizeof(BMPFileHeader), 1, file) != 1) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    if (fread(&bmp->info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    if (bmp->file_header.type != 0x4D42) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    if (bmp->info_header.bpp != 24) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    if (bmp->info_header.compression != 0) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    fseek(file, bmp->file_header.offset, SEEK_SET);

    int width = abs(bmp->info_header.width);
    int height = abs(bmp->info_header.height);
    bool top_down = bmp->info_header.height < 0;
    int row_padding = calculate_row_padding(width);

    bmp->image = image_create(width, height);
    if (!bmp->image) {
        free(bmp);
        fclose(file);
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        int target_y = top_down ? y : (height - 1 - y);
        
        for (int x = 0; x < width; x++) {
            uint8_t pixel[3];
            if (fread(pixel, 1, 3, file) != 3) {
                image_free(bmp->image);
                free(bmp);
                fclose(file);
                return NULL;
            }

            Color color = {
                pixel[2] / 255.0f,
                pixel[1] / 255.0f,
                pixel[0] / 255.0f
            };
            image_set_pixel(bmp->image, x, target_y, color);
        }

        fseek(file, row_padding, SEEK_CUR);
    }

    fclose(file);
    return bmp;
}

bool bmp_save(BMPImage* bmp, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return false;
    }

    int width = bmp->image->width;
    int height = bmp->image->height;
    int row_padding = calculate_row_padding(width);
    
    bmp->info_header.width = width;
    bmp->info_header.height = -height;
    bmp->info_header.image_size = (width * 3 + row_padding) * height;
    bmp->file_header.size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + bmp->info_header.image_size;

    fwrite(&bmp->file_header, sizeof(BMPFileHeader), 1, file);
    fwrite(&bmp->info_header, sizeof(BMPInfoHeader), 1, file);

    uint8_t padding[3] = {0, 0, 0};
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color color = image_get_pixel(bmp->image, x, y);
            uint8_t pixel[3] = {
                (uint8_t)(color.b * 255),
                (uint8_t)(color.g * 255),
                (uint8_t)(color.r * 255)
            };
            fwrite(pixel, 1, 3, file);
        }
        fwrite(padding, 1, row_padding, file);
    }

    fclose(file);
    return true;
}

void bmp_free(BMPImage* bmp) {
    if (bmp) {
        if (bmp->image) {
            image_free(bmp->image);
        }
        free(bmp);
    }
}

int calculate_row_padding(int width) {
    int row_size = width * 3;
    return (4 - (row_size % 4)) % 4;
}
