#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "compiler.h"
#include <float.h>
#include <stdarg.h>

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])



#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)
//


VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVm() {
    resetStack(vm);
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

static bool isFalsey(Value value) {
    return IS_NIL(value) || 
        (IS_BOOL(value) && !AS_BOOL(value));
}

Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

bool valueEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    
    switch (a.type)
    {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_NIL: return true;
        default:
            break;
    }
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->line[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
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
            BINARY_OP(NUMBER_VAL, +);
            break;

        case OP_SUB:
            BINARY_OP(NUMBER_VAL, -);
            break;
        
        case OP_MUL:
            BINARY_OP(NUMBER_VAL, *);
            break;

        case OP_DIV:
            BINARY_OP(NUMBER_VAL, /);
            break;

        case OP_NEGATE:
            if (!IS_NUMBER(peek(0))) {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }

            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;

        case OP_NIL: push(NIL_VAL); break;

        case OP_NOT: push(BOOL_VAL(isFalsey(pop()))); break;

        case OP_TRUE: push(BOOL_VAL(true)); break;

        case OP_FALSE: push(BOOL_VAL(false)); break;

        case OP_EQUAL: {
            Value b = pop();
            Value a = pop();
            push(BOOL_VAL(valueEqual(a, b)));
            break;
        }

        case OP_GREATER: 
            BINARY_OP(NUMBER_VAL, >);
            break;
        
        case OP_LESS:
            BINARY_OP(NUMBER_VAL, <);
            break;
    
        default:
            break;
        }
    }
    return INTERPRET_OK;
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