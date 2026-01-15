#include "image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

Image* image_create(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Image* image = (Image*)malloc(sizeof(Image));
    if (!image) {
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->data = (Color*)malloc(width * height * sizeof(Color));
    
    if (!image->data) {
        free(image);
        return NULL;
    }

    memset(image->data, 0, width * height * sizeof(Color));
    
    return image;
}

void image_free(Image* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

Image* image_clone(const Image* image) {
    Image* clone = image_create(image->width, image->height);
    if (!clone) {
        return NULL;
    }
    
    memcpy(clone->data, image->data, image->width * image->height * sizeof(Color));
    return clone;
}

Color image_get_pixel(const Image* image, int x, int y) {
    return image->data[y * image->width + x];
}

void image_set_pixel(Image* image, int x, int y, Color color) {
    image->data[y * image->width + x] = color;
}

Color image_get_pixel_clamped(const Image* image, int x, int y) {
    if (x < 0) x = 0;
    if (x >= image->width) x = image->width - 1;
    if (y < 0) y = 0;
    if (y >= image->height) y = image->height - 1;
    
    return image_get_pixel(image, x, y);
}

bool image_is_valid_coords(const Image* image, int x, int y) {
    return x >= 0 && x < image->width && y >= 0 && y < image->height;
}

int image_get_index(const Image* image, int x, int y) {
    return y * image->width + x;
}
