#include "test.h"


void addConst(Chunk* chunk, Value value) {
    writeChunk(chunk, OP_CONSTANT, 1);
    int constant = addConstants(chunk, value);
    writeChunk(chunk, constant, 1);
}

void unitTest1() {
    Chunk test;
    initChunk(&test);
    writeChunk(&test, OP_RETURN, 123);
    int constant = addConstants(&test, 567);
    writeChunk(&test, OP_CONSTANT, 123);
    writeChunk(&test, (uint8_t)constant, 123);
    disassembleChunk(&test, "Test Chunk");
};

void unitTest2() {
    Chunk test;
    initChunk(&test);
    addConst(&test, 12);
    addConst(&test, 13);

    addConst(&test, 10);
    writeChunk(&test, OP_ADD, 1);
    writeChunk(&test, OP_MUL, 1);
    writeChunk(&test, OP_RETURN, 1);
    interpret(&test);
};