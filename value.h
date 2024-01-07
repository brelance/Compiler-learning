#ifndef clox_values_h
#define clox_values_h
#include "common.h"
#include <float.h>


typedef uint64_t Value;

typedef struct {
    int count;
    int capacity;
    Value* values;
} ValueArray;

void initValue(ValueArray* values);
void writeValue(ValueArray* values, Value byte);
void freeValue(ValueArray* values);
void printValue(Value value);


#endif