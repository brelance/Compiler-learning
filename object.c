#include "object.h"
#include "memory.h"
#include <string.h>
#include "vm.h"


#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)


static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}


ObjString* allocateString(char* chars, int length) {
    ObjString* string = allocateObject(ObjString, OBJ_STRING);
    string->chars = chars;
    string->length = length;
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
    return allocateString(chars, length);
}


ObjString* copyString(const char* src, int length) {
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, src, length);
    chars[length] = '\0';
    return allocateString(chars, length);
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
