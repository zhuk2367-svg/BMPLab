#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bmp.h"
#include "image.h"
#include "filters.h"
#include "pipeline.h"

// Вывод справки по использованию программы
void print_help() {
    printf("Использование: image_craft <input.bmp> <output.bmp> [фильтры...]\n");
    printf("Пример: image_craft input.bmp output.bmp -crop 800 600 -gs -blur 0.5\n\n");
    printf("Доступные фильтры:\n");
    printf("  -crop <width> <height>       Обрезать изображение\n");
    printf("  -gs                          Оттенки серого\n");
    printf("  -neg                         Негатив\n");
    printf("  -sharp                       Повышение резкости\n");
    printf("  -edge <threshold>            Выделение границ\n");
    printf("  -med <window_size>           Медианный фильтр\n");
    printf("  -blur <sigma>                Размытие по Гауссу\n");
    printf("  -crystallize <cell_size>     Кристаллизация (дополнительный)\n");
    printf("  -glass <distortion>          Стеклянный эффект (дополнительный)\n");
}

int main(int argc, char* argv[]) {
    // Проверка минимального количества аргументов
    if (argc < 3) {
        print_help();
        return 0;
    }

    // Первые два аргумента - входной и выходной файлы
    char* input_file = argv[1];
    char* output_file = argv[2];

    // Загрузка исходного BMP-файла
    BMPImage* bmp = bmp_load(input_file);
    if (!bmp) {
        fprintf(stderr, "Ошибка загрузки файла: %s\n", input_file);
        return 1;
    }

    // Создание пайплайна фильтров (последовательности обработки)
    Pipeline* pipeline = pipeline_create();
    if (!pipeline) {
        fprintf(stderr, "Ошибка создания пайплайна\n");
        bmp_free(bmp);
        return 1;
    }

    // Парсинг аргументов командной строки (фильтров)
    // Начинаем с 3-го аргумента (после входного и выходного файлов)
    for (int i = 3; i < argc; i++) {
        // Проверяем каждый аргумент и добавляем соответствующий фильтр в пайплайн
        if (strcmp(argv[i], "-crop") == 0 && i + 2 < argc) {
            int width = atoi(argv[++i]);   // Следующий аргумент - ширина
            int height = atoi(argv[++i]);  // Еще следующий - высота
            pipeline_add_filter(pipeline, FILTER_CROP, width, height, 0);
        }
        else if (strcmp(argv[i], "-gs") == 0) {
            pipeline_add_filter(pipeline, FILTER_GRAYSCALE, 0, 0, 0);
        }
        else if (strcmp(argv[i], "-neg") == 0) {
            pipeline_add_filter(pipeline, FILTER_NEGATIVE, 0, 0, 0);
        }
        else if (strcmp(argv[i], "-sharp") == 0) {
            pipeline_add_filter(pipeline, FILTER_SHARPENING, 0, 0, 0);
        }
        else if (strcmp(argv[i], "-edge") == 0 && i + 1 < argc) {
            float threshold = atof(argv[++i]);  // Порог для обнаружения границ
            pipeline_add_filter(pipeline, FILTER_EDGE_DETECTION, 0, 0, threshold);
        }
        else if (strcmp(argv[i], "-med") == 0 && i + 1 < argc) {
            int window = atoi(argv[++i]);  // Размер окна медианного фильтра
            pipeline_add_filter(pipeline, FILTER_MEDIAN, window, 0, 0);
        }
        else if (strcmp(argv[i], "-blur") == 0 && i + 1 < argc) {
            float sigma = atof(argv[++i]);  // Сигма для Гауссова размытия
            pipeline_add_filter(pipeline, FILTER_GAUSSIAN_BLUR, 0, 0, sigma);
        }
        else if (strcmp(argv[i], "-crystallize") == 0 && i + 1 < argc) {
            int cell_size = atoi(argv[++i]);  // Размер ячейки кристаллизации
            pipeline_add_filter(pipeline, FILTER_CRYSTALLIZE, cell_size, 0, 0);
        }
        else if (strcmp(argv[i], "-glass") == 0 && i + 1 < argc) {
            float distortion = atof(argv[++i]);  // Уровень искажения стеклянного эффекта
            pipeline_add_filter(pipeline, FILTER_GLASS, 0, 0, distortion);
        }
        else {
            fprintf(stderr, "Неизвестный фильтр или неверные параметры: %s\n", argv[i]);
            pipeline_free(pipeline);
            bmp_free(bmp);
            return 1;
        }
    }

    // Применение всех фильтров пайплайна к изображению
    Image* processed_image = pipeline_apply(pipeline, bmp->image);
    if (!processed_image) {
        fprintf(stderr, "Ошибка применения фильтров\n");
        pipeline_free(pipeline);
        bmp_free(bmp);
        return 1;
    }

    // Обновление изображения в структуре BMP
    // Освобождаем старое изображение и заменяем обработанным
    free(bmp->image->data);
    free(bmp->image);
    bmp->image = processed_image;
    
    // Обновление размеров в заголовке BMP
    bmp->info_header.width = processed_image->width;
    bmp->info_header.height = -processed_image->height;  // Отрицательная высота для top-down

    // Сохранение результата в файл
    if (!bmp_save(bmp, output_file)) {
        fprintf(stderr, "Ошибка сохранения файла: %s\n", output_file);
        pipeline_free(pipeline);
        bmp_free(bmp);
        return 1;
    }

    printf("Обработка завершена успешно!\n");

    // Освобождение всех ресурсов
    pipeline_free(pipeline);
    bmp_free(bmp);

    return 0;
}
