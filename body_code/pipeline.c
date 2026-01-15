// pipeline.c
#include "pipeline.h"
#include <stdlib.h>
//СОЗДАНИЕ И УДАЛЕНИЕ ПАЙПЛАЙНА

//*создает новый пайплайн фильтров
Pipeline* pipeline_create(void) {
    Pipeline* pipeline = (Pipeline*)malloc(sizeof(Pipeline));
    if (!pipeline) {
        return NULL;
    }
    
    pipeline->head = NULL;
    pipeline->tail = NULL;
    pipeline->count = 0;
    
    return pipeline;
}
//*освобождает память
void pipeline_free(Pipeline* pipeline) {
    if (!pipeline) return;
    
    PipelineNode* current = pipeline->head;
    while (current) {
        PipelineNode* next = current->next;
        free(current);
        current = next;
    }
    
    free(pipeline);
}

//ДОБАВЛЕНИЕ ФИЛТРОВ

//*добавление фильтра в конец пайплайна
void pipeline_add_filter(Pipeline* pipeline, FilterType type, int param1, int param2, float param3) {
    if (!pipeline) return;
    
    PipelineNode* node = (PipelineNode*)malloc(sizeof(PipelineNode));
    if (!node) return;
    
    node->filter.type = type;
    node->filter.param1 = param1;
    node->filter.param2 = param2;
    node->filter.param3 = param3;
    node->next = NULL;
    
    if (!pipeline->head) {
        pipeline->head = node;
        pipeline->tail = node;
    } else {
        pipeline->tail->next = node;
        pipeline->tail = node;
    }
    
    pipeline->count++;
}

//ПРИМЕНЕНИЕ ПАЙПЛАЙНА К ИЗОБРАЖЕНИЮ
//*применяет все фильтры к изображению 
Image* pipeline_apply(Pipeline* pipeline, const Image* image) {
    if (!pipeline || !image) return NULL;
    
    Image* current = image_clone(image);
    if (!current) return NULL;
    
    PipelineNode* node = pipeline->head;
    while (node) {
        Image* next = filter_apply(&node->filter, current);
        if (!next) {
            image_free(current);
            return NULL;
        }
        
        image_free(current);
        current = next;
        node = node->next;
    }
    
    return current;

}
