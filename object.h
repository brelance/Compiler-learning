#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

#define IS_STRING(value) isObjType(value, OBJ_STRING)




typedef enum {
    OBJ_STRING,
} ObjType;


struct Obj
{
    ObjType type;
    struct Obj* next;
};

struct ObjString
{
    Obj obj;
    char* chars;
    int length;
};

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);

void printObj(Value value);

#endif