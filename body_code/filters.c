#include "filters.h"
#include <math.h>
#include <string.h>
#include <time.h>

// Вспомогательная функция для сравнения float (используется в qsort)
static int compare_floats(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);  // Возвращает -1, 0, 1
}

// Ограничение значений цвета в диапазоне [0.0, 1.0]
static Color color_clamp(Color c) {
    if (c.r < 0.0f) c.r = 0.0f;
    if (c.g < 0.0f) c.g = 0.0f;
    if (c.b < 0.0f) c.b = 0.0f;
    if (c.r > 1.0f) c.r = 1.0f;
    if (c.g > 1.0f) c.g = 1.0f;
    if (c.b > 1.0f) c.b = 1.0f;
    return c;
}

// Фильтр обрезки (Crop)
// Вырезает прямоугольную область из изображения
Image* filter_apply_crop(const Image* image, int width, int height) {
    // Определяем новые размеры (не больше исходных)
    int new_width = (width < image->width) ? width : image->width;
    int new_height = (height < image->height) ? height : image->height;
    
    // Создаем новое изображение
    Image* result = image_create(new_width, new_height);
    if (!result) {
        return NULL;
    }
    
    // Копируем пиксели из верхнего левого угла
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            Color color = image_get_pixel(image, x, y);
            image_set_pixel(result, x, y, color);
        }
    }
    
    return result;
}

// Фильтр оттенков серого (Grayscale)
// Преобразует цветное изображение в черно-белое
Image* filter_apply_grayscale(const Image* image) {
    // Создаем копию изображения
    Image* result = image_clone(image);
    if (!result) {
        return NULL;
    }
    
    // Преобразуем каждый пиксель
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);
            // Формула преобразования RGB в оттенки серого
            // Коэффициенты учитывают восприятие цвета человеком
            float gray = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
            Color gray_color = {gray, gray, gray};
            image_set_pixel(result, x, y, gray_color);
        }
    }
    
    return result;
}

// Фильтр негатива (Negative)
// Инвертирует цвета изображения
Image* filter_apply_negative(const Image* image) {
    Image* result = image_clone(image);
    if (!result) {
        return NULL;
    }
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);
            // Инвертируем каждый канал: новый = 1.0 - исходный
            Color negative = {1.0f - color.r, 1.0f - color.g, 1.0f - color.b};
            image_set_pixel(result, x, y, negative);
        }
    }
    
    return result;
}

// Фильтр повышения резкости (Sharpening)
// Усиливает контраст на границах объектов
Image* filter_apply_sharpening(const Image* image) {
    Image* result = image_clone(image);
    if (!result) {
        return NULL;
    }
    
    // Ядро свертки для повышения резкости
    // Центральный элемент усилен для выделения деталей
    float kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    
    // Применяем свертку с ядром
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color sum = {0, 0, 0};
            
            // Проходим по окрестности 3x3
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    // Берем пиксель с учетом границ
                    Color neighbor = image_get_pixel_clamped(image, x + kx, y + ky);
                    float weight = kernel[ky + 1][kx + 1];
                    
                    // Суммируем взвешенные значения
                    sum.r += neighbor.r * weight;
                    sum.g += neighbor.g * weight;
                    sum.b += neighbor.b * weight;
                }
            }
            
            // Ограничиваем результат и сохраняем
            image_set_pixel(result, x, y, color_clamp(sum));
        }
    }
    
    return result;
}

// Фильтр обнаружения границ (Edge Detection)
// Выделяет границы объектов на изображении
Image* filter_apply_edge_detection(const Image* image, float threshold) {
    // Сначала преобразуем в оттенки серого
    Image* grayscale = filter_apply_grayscale(image);
    if (!grayscale) {
        return NULL;
    }
    
    Image* result = image_create(image->width, image->height);
    if (!result) {
        image_free(grayscale);
        return NULL;
    }
    
    // Ядро Лапласа для обнаружения границ
    float kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            float sum = 0.0f;
            
            // Свертка с ядром Лапласа
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    Color neighbor = image_get_pixel_clamped(grayscale, x + kx, y + ky);
                    float weight = kernel[ky + 1][kx + 1];
                    sum += neighbor.r * weight;  // Все каналы одинаковы в grayscale
                }
            }
            
            // Бинаризация по порогу
            Color color;
            if (sum > threshold) {
                color.r = color.g = color.b = 1.0f;  // Белый - граница
            } else {
                color.r = color.g = color.b = 0.0f;  // Черный - фон
            }
            
            image_set_pixel(result, x, y, color);
        }
    }
    
    image_free(grayscale);
    return result;
}

// Медианный фильтр (Median Filter)
// Удаляет шум, сохраняя границы
Image* filter_apply_median(const Image* image, int window) {
    Image* result = image_create(image->width, image->height);
    if (!result) {
        return NULL;
    }
    
    int radius = window / 2;  // Радиус окна
    int window_size = window * window;  // Общее количество пикселей в окне
    
    // Буферы для хранения значений каналов в окне
    float* reds = (float*)malloc(window_size * sizeof(float));
    float* greens = (float*)malloc(window_size * sizeof(float));
    float* blues = (float*)malloc(window_size * sizeof(float));
    
    if (!reds || !greens || !blues) {
        free(reds);
        free(greens);
        free(blues);
        image_free(result);
        return NULL;
    }
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int count = 0;
            
            // Собираем все пиксели в окне
            for (int wy = -radius; wy <= radius; wy++) {
                for (int wx = -radius; wx <= radius; wx++) {
                    Color color = image_get_pixel_clamped(image, x + wx, y + wy);
                    reds[count] = color.r;
                    greens[count] = color.g;
                    blues[count] = color.b;
                    count++;
                }
            }
            
            // Сортируем значения каждого канала
            qsort(reds, count, sizeof(float), compare_floats);
            qsort(greens, count, sizeof(float), compare_floats);
            qsort(blues, count, sizeof(float), compare_floats);
            
            // Берем медиану (серединное значение)
            int mid = count / 2;
            Color median = {reds[mid], greens[mid], blues[mid]};
            image_set_pixel(result, x, y, median);
        }
    }
    
    free(reds);
    free(greens);
    free(blues);
    
    return result;
}


// Общая функция применения фильтра
// Вызывает соответствующую функцию фильтра по типу
Image* filter_apply(const Filter* filter, const Image* image) {
    switch (filter->type) {
        case FILTER_CROP:
            return filter_apply_crop(image, filter->param1, filter->param2);
        case FILTER_GRAYSCALE:
            return filter_apply_grayscale(image);
        case FILTER_NEGATIVE:
            return filter_apply_negative(image);
        case FILTER_SHARPENING:
            return filter_apply_sharpening(image);
        case FILTER_EDGE_DETECTION:
            return filter_apply_edge_detection(image, filter->param3);
        case FILTER_MEDIAN:
            return filter_apply_median(image, filter->param1);
        default:
            return NULL;  // Неизвестный тип фильтра
    }

}
