#ifndef FILTERS_H
#define FILTERS_H

#include "image.h"
#include <stdlib.h>

// Типы фильтров
typedef enum {
    FILTER_CROP,           // Обрезка
    FILTER_GRAYSCALE,      // Оттенки серого
    FILTER_NEGATIVE,       // Негатив
    FILTER_SHARPENING,     // Повышение резкости
    FILTER_EDGE_DETECTION, // Обнаружение границ
    FILTER_MEDIAN,         // Медианный фильтр
    FILTER_GAUSSIAN_BLUR,  // Гауссово размытие
    FILTER_CRYSTALLIZE,    // Кристаллизация
    FILTER_GLASS           // Стеклянный эффект
} FilterType;

// Структура для параметров фильтра
// param1, param2 - целочисленные параметры
// param3 - параметр с плавающей точкой
typedef struct {
    FilterType type;  // Тип фильтра
    int param1;       // Например: ширина для crop, размер окна для median
    int param2;       // Например: высота для crop
    float param3;     // Например: порог для edge detection, sigma для blur
} Filter;

// Функции фильтров (каждая применяет соответствующий фильтр)
Image* filter_apply_crop(const Image* image, int width, int height);
Image* filter_apply_grayscale(const Image* image);
Image* filter_apply_negative(const Image* image);
Image* filter_apply_sharpening(const Image* image);
Image* filter_apply_edge_detection(const Image* image, float threshold);
Image* filter_apply_median(const Image* image, int window);
Image* filter_apply_gaussian_blur(const Image* image, float sigma);
Image* filter_apply_crystallize(const Image* image, int cell_size);
Image* filter_apply_glass(const Image* image, float distortion);

// Общая функция применения фильтра
// Выбирает нужную функцию по типу фильтра
Image* filter_apply(const Filter* filter, const Image* image);

#endif // FILTERS_H
