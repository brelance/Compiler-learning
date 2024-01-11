#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "compiler.h"
#include <float.h>
#include <stdarg.h>
#include <string.h>
#include "memory.h"
#include "object.h"

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])
#define READ_STRING() \
    AS_STRING(READ_CONSTANT())


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
    vm.objects = NULL;
    initTable(&vm.strings);
    initTable(&vm.globals);

}


void freeVm() {
    freeObjects();
    freeTable(&vm.strings);
    freeTable(&vm.globals);
}


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

static void concatenate() {
    ObjString* bString = AS_STRING(peek(0));
    ObjString* aString = AS_STRING(peek(1));
    
    int length = aString->length + bString->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, aString->chars, aString->length);
    memcpy(chars + aString->length, bString->chars, bString->length);
    chars[length] = '\0';

    pop();
    pop();

    ObjString* string = takeString(chars, length);
    push(OBJ_VAL(string));
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
            return INTERPRET_OK;
        
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }

        case OP_ADD:
            if(IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if(IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                BINARY_OP(NUMBER_VAL, +);
            } else {
                runtimeError(
                    "Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
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
            BINARY_OP(BOOL_VAL, >);
            break;
        
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;

        case OP_PRINT:
            printValue(pop());
            printf("\n");
            break;

        case OP_POP: pop(); break;

        case OP_DEFINE_GLOBAL: {
            ObjString* name = READ_STRING();
            tableSet(&vm.globals, name, peek(0));
            pop();
            break;
        }
        
        case OP_GET_GLOBAL: {
            ObjString* name = READ_STRING();
            Value value;

            if (!tableGet(&vm.globals, name, &value)) {
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }

            push(value);
            break;;
        }

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