#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "file.h"

typedef enum {
    OP_INC,
    OP_DEC,
    OP_SHIFT_RIGHT,
    OP_SHIFT_LEFT,
    OP_PRINT_CHAR,
    OP_GET_CHAR,
    OP_FWD_JMP,
    OP_BCK_JMP,
    OP_EOF
} Op;

typedef struct _Command {
    Op op;

    // for brackets
    struct _Command* operand;
} Command;

#define PUSH_CMD(commands, length, op) \
    commands[length] = ((Command) { op, NULL }); length++;

#define PUSH_CMD_BCKT(commands, length, op, operand) \
    commands[length] = ((Command) { op, operand }); length++;

typedef struct _Node {
    Command* cmd;
    struct _Node* prev;
} Node;

Node* stack;

void stack_push(Command* cmd) {
    Node* node = malloc(sizeof(Node)); 
    node->cmd = cmd; 
    node->prev = stack; 
    stack = node;
}

Command* stack_pop() {
    Node* tail = stack;
    stack = tail->prev;
    Command* cmd = tail->cmd;
    free(tail);
    return cmd;
}

Command* bf_compile(const char* src, size_t length) {
    char* pc = (char*) src;
    Command* cmds = malloc(sizeof(Command) * length); // store cmd for each char
    int l = 0;

    while (*pc != '\0') {
        switch (*pc) {
            case ',': PUSH_CMD(cmds, l, OP_GET_CHAR); break;
            case '.': PUSH_CMD(cmds, l, OP_PRINT_CHAR); break;
            case '+': PUSH_CMD(cmds, l, OP_INC); break;
            case '-': PUSH_CMD(cmds, l, OP_DEC); break;
            case '<': PUSH_CMD(cmds, l, OP_SHIFT_LEFT); break;
            case '>': PUSH_CMD(cmds, l, OP_SHIFT_RIGHT); break;
            case '[': stack_push(&cmds[l]); PUSH_CMD(cmds, l, OP_FWD_JMP); break;
            case ']': {
                Command* cmd = stack_pop();
                cmd->operand = &cmds[l];
                PUSH_CMD_BCKT(cmds, l, OP_BCK_JMP, cmd);
                break;
            } 
        }
        pc++;
    }

    PUSH_CMD(cmds, l, OP_EOF);

    return cmds;
}

void interpret(Command* program) {
    unsigned char tape[30000] = {0};
    unsigned char* ptr = tape;

    while (program->op != OP_EOF) {
        switch (program->op) {
            case OP_INC: (*ptr)++; break;
            case OP_DEC: (*ptr)--; break;
            case OP_SHIFT_RIGHT: ptr++; break;
            case OP_SHIFT_LEFT: ptr--; break;
            case OP_PRINT_CHAR: printf("%c", *ptr); break;
            case OP_GET_CHAR: { 
                char ch = getchar(); 
                *ptr = ch; 
                break;
            }
            case OP_FWD_JMP: if (!*ptr) program = program->operand; break;
            case OP_BCK_JMP: if (*ptr) program = program->operand; break;
            default: break; // unreachable
        }
        program++;
    }
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        printf("<interpreter> file.bf\n");
        return -1;
    }

    const char* path = argv[1];
    char* src = read_file(path);
    size_t length = strlen(src);
    Command* program = bf_compile(src, length);
    interpret(program);
}