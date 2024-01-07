#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "compiler.h"

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])

#define BINARY_OP(op) \
    do { \
        Value b = pop(); \
        Value a = pop(); \
        push(a op b); \
    } while(false)


VM vm;

static void restStack() {
    vm.stackTop = vm.stack;
}

void initVm() {
    restStack(vm);
}


void freeVm();


void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

InterpretResult run() {
    for(;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            disassembleChunk(vm.chunk, "==Stack-Trace==");
            // printf("==Stack-Trace==");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[");
                printValue(*slot);
                printf("]");
            }
            printf("\n");
        #endif
        switch (READ_BYTE())
        {
        case OP_RETURN:
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }

        case OP_ADD:
            BINARY_OP(+);
            break;

        case OP_SUB:
            BINARY_OP(-);
            break;
        
        case OP_MUL:
            BINARY_OP(*);
            break;

        case OP_DIV:
            BINARY_OP(/);
            break;

        case OP_NEGATE:
            push(-pop());
            break;
        default:
            break;
        }

    }
}


InterpretResult interpret(const char* source) {
    Chunk chunk;
    initVm();
    initChunk(&chunk);

    if(!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    
    freeChunk(&chunk);
    return result;
}