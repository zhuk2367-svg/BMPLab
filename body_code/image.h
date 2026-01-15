#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>

typedef struct {
    float r, g, b;
} Color;

typedef struct {
    int width;
    int height;
    Color* data;
} Image;

// Создание и освобождение
Image* image_create(int width, int height);
void image_free(Image* image);
Image* image_clone(const Image* image);

// Доступ к пикселям
Color image_get_pixel(const Image* image, int x, int y);
void image_set_pixel(Image* image, int x, int y, Color color);
Color image_get_pixel_clamped(const Image* image, int x, int y);

// Утилиты
bool image_is_valid_coords(const Image* image, int x, int y);
int image_get_index(const Image* image, int x, int y);


#endif 
