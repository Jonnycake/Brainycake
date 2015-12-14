#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define MIN_LOAD 1
#include <brainycake.h>


// Current processing mode
char mode = MODE_START;

// Error reporting modes
char quiet = 0;
char verbose = 0;
char debug = 0;
char superverbose = 0;

/**
 * Preprocesses the main code file using bc_include() for includes
 */
int
bc_preprocess(char* mainfile, char** code)
{
    char c;
    char error = ERROR_NORMAL;
    FILE* f = fopen(mainfile, "rb");
    if(!f) {
        return ERROR_BADFILE;
    }
    int codepos = 0;
    struct stat filestat;
    stat(mainfile, &filestat);
    int size = filestat.st_size;
    *code = calloc(sizeof(char), size+10);

    mode = MODE_PREPROCESS;

    while( !error && (c = fgetc(f)) != EOF) {
        switch(mode)
        {
            case MODE_COMMENT_ML:
                 if( c == '/' ) {
                     mode = MODE_PREPROCESS;
                 }
                 break;
            case MODE_COMMENT_SL:
                 if( c == '\n' ) {
                     mode = MODE_PREPROCESS;
                 }
                 break;
            case MODE_OUTPUT:
                 (*code)[codepos++] = c;
                 if( c == '\\' ) {
                     mode = MODE_PREPROCESS;
                 }
                 break;
            case MODE_PREPROCESS:
                switch(c)
                {
                    case '#':
                        mode = MODE_COMMENT_SL;
                        break;
                    case '/':
                        mode = MODE_COMMENT_ML;
                        break;
                    case '\\':
                        mode = MODE_OUTPUT;
                        (*code)[codepos++] = c;
                        break;
                    default:
                        if( (!isspace(c) || mode == MODE_OUTPUT)
                            && mode != MODE_COMMENT_SL
                            && mode != MODE_COMMENT_ML
                        ) {
                            (*code)[codepos++] = c;
                        }
                        break;
                }
        }
    }
    return error;
}

int
bc_include(char* file, char** functions, char** code, int cursize)
{
    FILE* f;
}

int
bc_execute(char* code)
{
    // Basic variables
    int codelen = strlen(code);
    int codepos = 0;
    char c;
    int loop_positions[MAX_LOOPS] = {0};
    unsigned char numloops = 0;
    char* tape = calloc(sizeof(char), INIT_CELL_COUNT);
    char** a;
    int curCellCount = INIT_CELL_COUNT;
    int p = 0;
    int error = ERROR_NORMAL;
    char* regArgv;
    int regArgc = 0;

    // Set our mode to execution
    mode = MODE_EXEC;

    // Memory objects
    Registry registry;
    Registry_construct(&registry, (int*) 0, (int*) 0, (int*) 0, (int*)tape);
    a = (char**)(&registry.extregisters[TAPE_PTR]);
    Stack s;
    Stack_construct(&s, MAX_STACK_HEIGHT);

    for( ; codepos <= codelen; c = code[codepos++]) {
        if(verbose + superverbose) {
            bc_debug(&s, &registry, tape, curCellCount);
        }
        switch(mode)
        {
            case MODE_OUTPUT:
                if( c == '\\' ) {
                    mode = MODE_EXEC;
                } else {
                    putchar(c);
                }
                break;
            case MODE_REG_MANIP_ADD:
            case MODE_REG_MANIP_SUB:
            case MODE_REG_MANIP_MOD:
            case MODE_REG_MANIP_MUL:
            case MODE_REG_MANIP_DIV:
            case MODE_REG_MANIP_SET:
            case MODE_REG_MANIP_NOT:
            case MODE_REG_MANIP_DONE:
            case MODE_REG_MANIP:
                //error = bc_reg_manip(&registry);
                break;
            case MODE_EXEC:
                switch(c)
                {
                    // Traditional brainfuck
                    case '>':
                        if((++(*a) - tape) >= curCellCount) {
                            // Provide wrap around rather than error out
                            if((curCellCount + INIT_CELL_COUNT) > MAX_CELL_COUNT) {
                                *a = tape;
                            } else {
                                curCellCount += INIT_CELL_COUNT;
                                tape = realloc(tape, curCellCount * sizeof(char));
                            }
                        }
                        break;
                    case '<':
                        if(--(*a) < tape) {
                            *a = tape + curCellCount - 1;
                        }
                        break;
                    case '.':
                        putchar(**a);
                        break;
                    case ',':
                        **a = getchar();
                        break;
                    case '+':
                        (**a)++;
                        break;
                    case '-':
                        (**a)--;
                        break;
                    case '[':
                        if((**a) && (numloops == 0 || loop_positions[numloops - 1] != codepos)) {
                            loop_positions[numloops++] = codepos;
                        } else if(!(**a)) {
                            codepos++;
                            int lastloop = numloops++;
                            for( ; numloops != lastloop && codepos != codelen ; codepos++) {
                                if(code[codepos] == '[') {
                                    numloops++;
                                } else if(code[codepos] == ']') {
                                    numloops--;
                                }
                            }
                        }
                        break;
                    case ']':
                        if((**a)) {
                            codepos = loop_positions[numloops - 1];
                        } else {
                            if(numloops != 0) {
                                loop_positions[--numloops] = 0;
                            }
                        }
                        break;

                    // Shortcuts
                    case '_':
                        *a = tape;
                        break;
                    case '^':
                        (**a) = 0;
                        break;
                    case '|':
                        {
                            char v[5] = {0};
                            int count = 0;
                            for( c = code[codepos++];
                                 c != '|' && count < 4;
                                 c = code[codepos++], count++
                            ) {
                                if((c >= 48 && c < 58) || (count == 0 && c == '-')) {
                                    v[count] = c;
                                } else {
                                    error = ERROR_UNKNOWN;
                                    break;
                                }
                            }
                            if(c == '|') {
                                count = atoi(v);
                                if(count > 255 && !quiet) {
                                    printf("Warning: Number is greater than 255, there may be unexpected results.\n");
                                }
                                (**a) += atoi(v);
                            } else {
                                printf("Error: Requested number is out of bounds.\n");
                        }
                    }
                    break;
                    case '\\':
                        mode = MODE_OUTPUT;
                        break;

                    // Registers
                    case '$':
                        c = code[codepos++];
                        if( c == EOF ) {
                            printf("Error: Scanned to end of file when attempting to identify a register.");
                        } else {
                            switch(c)
                            {
                                case '0':
                                    printf("Error: Attempted to write to read-only register ($0).");
                                    error = ERROR_RO;
                                    break;
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    registry.setRegister(&registry, c, (**a));
                                    break;
                                case '{':
                                    mode = MODE_REG_MANIP;
                                    break;
                                case '+':
                                case '-':
                                case '/':
                                case '*':
                                case '%':
                                case '&':
                                case '|':
                                case '^':
                                case '!':
                                    regArgc = 0;
                                    registry.performOperation(&registry, c, regArgv, regArgc);
                                    break;
                                default:
                                    printf("Syntax error: Invalid register operation '%c'.\n", c);
                                    error = ERROR_BADOP;
                                    break;
                            }
                        }
                        break;
                    case 's':
                        regArgc = 0;
                        registry.performOperation(&registry, ',', regArgv, regArgc);
                        break;
                    case 'p':
                        {
                         int val;
                         c = code[codepos++];
                         if( c == EOF ) {
                             printf("Error: Scanned to end of file when attempting to identify register to print.");
                         } else {
                             switch(c)
                             {
                                 case '0':
                                     printf("NULL");
                                     break;
                                 case '1':
                                 case '2':
                                 case '3':
                                 case '4':
                                     error = registry.getRegisterValue(&registry, c, &val);
                                     printf("%c", val);
                                     break;
                                 case '5':
                                 case '6':
                                 case '7':
                                 case '8':
                                 case '9':
                                     error = registry.getRegisterValue(&registry, c, &val);
                                     printf("%d", val);
                                     break;
                                 default:
                                     printf("Error: Unknown register '%c'.", c);
                                     error = ERROR_UNKREG;
                                     break;
                             }
                         }
                        }
                        break;

                    // Stack
                    case '"':
                        bc_push(&s, tape, (*a) - tape);
                        break;

                    case '\'':
                        bc_pop(&s, tape, (*a) - tape);
                        break;

                    // Debugging
                    case 'h':
                        printf("0x%x", ((**a) >= 0) ? (**a) : 256 + (**a));
                        break;
                    case 'd':
                        printf("%d", ((**a) >= 0) ? (**a) : 256 + (**a));
                        break;
                    case 'v':
                        verbose = (verbose) ? 0 : 1;
                        break;
                    case '@':
                        error = ERROR_PROG;
                        break;
                    case '?':
                    case '!':
                        break;
                }
        }

        if(verbose + superverbose) {
            bc_debug(&s, &registry, tape, curCellCount);
        }
    }
    if(debug) {
        bc_debug(&s, &registry, tape, curCellCount);
    }
    s.destruct(&s);
    registry.destruct(&registry);
    return error;
}

int
bc_reg_manip(Registry* r, char* manip_command)
{
}

void
bc_jump(char* code, int* codepos)
{
}

void
bc_push(Stack* s, char* a, int p)
{
    int v = 0;
    int x = sizeof(int) - 1;
    for( ; x >= 0 ; x-- ) {
        v |= a[p++] << (x * 8);
    }
    s->push(s, v);
}

void bc_pop(Stack* s, char* a, int p)
{
    int i = p;
    int x = sizeof(int) - 1;
    int v;
    s->pop(s, &v);
    for( ; x >= 0 ; x-- ) {
        a[i++] = (v >> (x * 8)) & 0xFF;
    }
}

void bc_debug(Stack* s, Registry* r, signed char* tape, int curCellCount)
{
    int curCell = 0;
    printf("=======    Tape   ======\n");
    for( ; curCell < curCellCount ; curCell++) {
        if(tape[curCell] != (char) 0) {
            printf("Cell #%*d: %x\n", 0 - (int)ceil(log10(curCellCount)), curCell + 1, tape[curCell]);
        }
    }
    curCell = (signed char*) r->extregisters[TAPE_PTR] - tape;
    printf("Current Cell (%d/%d): %x\n", curCell + 1, curCellCount, tape[curCell]);
    printf("======= Registers ======\n");
    r->printRegisters(r);
    printf("========   Stack   =====\n");
    s->printStack(s);
}
