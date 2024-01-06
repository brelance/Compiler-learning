#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"


typedef enum {
    OP_CONSTANT,
    OP_RETURN,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEGATE,
} OpCode;


typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    int* line;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* Chunk, uint8_t byte, int line);
void freeChunk(Chunk* chunk);
int addConstants(Chunk* chunk, Value value);




#endif