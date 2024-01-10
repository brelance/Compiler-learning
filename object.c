#include "object.h"
#include "memory.h"
#include <string.h>
#include "vm.h"
#include "table.h"
#include <stdio.h>


#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)


static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* allocateString(char* chars, int length, uint32_t hash) {
    ObjString* string = (ObjString*)allocateObject(sizeof(ObjString), OBJ_STRING);
    string->chars = chars;
    string->length = length;
    string->hash = hash;
    
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
}


// ObjString* allocateString(char* chars, int length) {
//     ObjString* string = ALLOCATE(ObjString, 1);
//     string->obj.type = OBJ_STRING;
//     string->chars = chars;
//     string->length = length;
//     return string;
// }

ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);

    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1); // Wait test FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(chars, length, hash);
}


ObjString* copyString(const char* src, int length) {
    uint32_t hash = hashString(src, length);
    ObjString* interned = tableFindString(&vm.strings, src, length, hash);

    if (interned != NULL) return interned;
    
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, src, length);
    chars[length] = '\0';
    return allocateString(chars, length, hash);
}


void printObj(Value value) {
    switch (OBJ_TYPE(value))
    {
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    
    default:
        break;
    }
}
