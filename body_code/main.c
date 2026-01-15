#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bmp.h"
#include "image.h"

void print_help() {
    printf("Использование: image_craft <input.bmp> <output.bmp>\n");
    printf("Пример: image_craft input.bmp output.bmp\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_help();
        return 0;
    }

    char* input_file = argv[1];
    char* output_file = argv[2];

    BMPImage* bmp = bmp_load(input_file);
    if (!bmp) {
        fprintf(stderr, "Ошибка загрузки файла: %s\n", input_file);
        return 1;
    }

    if (!bmp_save(bmp, output_file)) {
        fprintf(stderr, "Ошибка сохранения файла: %s\n", output_file);
        bmp_free(bmp);
        return 1;
    }

    printf("Обработка завершена успешно!\n");

    bmp_free(bmp);

    return 0;
}
