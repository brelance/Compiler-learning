#include "value.h"
#include "memory.h"
#include <stdio.h>
#include <inttypes.h>
#include "object.h"
#include <string.h>
#include "vm.h"

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

bool valueEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    
    switch (a.type)
    {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_NIL: return true;
        case VAL_OBJ: {
            // ObjString* bString = AS_STRING(pop());
            // ObjString* aString = AS_STRING(pop());

            // return (aString->length == bString->length &&
            //     memcmp(aString->chars, bString->chars,aString->length) == 0);
            return AS_OBJ(a) == AS_OBJ(b);
        }
        default:
            break;
    }
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ: printObj(value);
    }
}