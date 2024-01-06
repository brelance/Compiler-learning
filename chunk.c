#include "chunk.h"
#include "common.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->line = NULL;
    chunk->code = NULL;
    initValue(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity,chunk->capacity);
        chunk->line = GROW_ARRAY(int, chunk->line,
            oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;   
    chunk->line[chunk->count] = line;
    chunk->count++;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->count);
    FREE_ARRAY(int, chunk->line, chunk->capacity);
    freeValue(&chunk->constants);
    initChunk(chunk);
}

int addConstants(Chunk* chunk, Value value) {
    writeValue(&chunk->constants, value);
    return chunk->constants.count - 1;
}
