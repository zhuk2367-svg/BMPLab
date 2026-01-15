// pipeline.h
#ifndef PIPELINE_H
#define PIPELINE_H

#include "filters.h"

// Структура для узла пайплайна
typedef struct PipelineNode {
    Filter filter;
    struct PipelineNode* next;
} PipelineNode;

// Структура пайплайна
typedef struct {
    PipelineNode* head;
    PipelineNode* tail;
    int count;
} Pipeline;

// Функции пайплайна
Pipeline* pipeline_create(void);
void pipeline_free(Pipeline* pipeline);
void pipeline_add_filter(Pipeline* pipeline, FilterType type, int param1, int param2, float param3);
Image* pipeline_apply(Pipeline* pipeline, const Image* image);

#endif // PIPELINE_H