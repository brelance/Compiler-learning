#include "value.h"
#include "memory.h"
#include <stdio.h>
#include <inttypes.h>

void initValue(ValueArray* array) {
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValue(ValueArray* array, Value byte) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values,
            oldCapacity,array->capacity);
    }

    array->values[array->count] = byte;
    array->count++;
}

void freeValue(ValueArray* array) {
    FREE_ARRAY(Value, array->values ,array->capacity);
    initValue(array);
}

void printValue(Value value) {
    printf("%" PRIu64 "", value);
}