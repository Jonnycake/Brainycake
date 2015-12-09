#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    char error = 0;
    FILE* f = fopen(mainfile, "rb");
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
    int codelen = strlen(code);
    int codepos = 0;
    char c;
    char registers[5] = {0};
    int extregisters[5] = {0};

    int loop_positions[MAX_LOOPS] = {0};
    unsigned char numloops = 0;
    char* a = calloc(sizeof(char), INIT_CELL_COUNT);
    int curCellCount = INIT_CELL_COUNT;
    int p = 0;
    int error = 0;
    Registry registry;
    Registry_construct(&registry);

    Stack s;
    Stack_construct(&s, MAX_STACK_HEIGHT);
    mode = MODE_EXEC;
    for( ; codepos <= codelen; c = code[codepos++]) {
        if(verbose + superverbose) {
            printf("\n\nINSTRUCTION: %c\nCurrent Stats: %d - %c - %d\n\n", c, p, a[p], a[p]);
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
                        if(++p >= curCellCount) {
                            // Provide wrap around rather than error out
                            if((curCellCount + INIT_CELL_COUNT) > MAX_CELL_COUNT) {
                                p = 0;
                            } else {
                                curCellCount += INIT_CELL_COUNT;
                                a = realloc(a, curCellCount * sizeof(char));
                            }
                        }
                        break;
                    case '<':
                        if(--p < 0) {
                            p = curCellCount - 1;
                        }
                        break;
                    case '.':
                        putchar(a[p]);
                        break;
                    case ',':
                        a[p] = getchar();
                        break;
                    case '+':
                        a[p]++;
                        break;
                    case '-':
                        a[p]--;
                        break;
                    case '[':
                        if(a[p] && (numloops == 0 || loop_positions[numloops - 1] != codepos)) {
                            loop_positions[numloops++] = codepos;
                        } else if(!a[p]) {
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
                        if(a[p]) {
                            codepos = loop_positions[numloops - 1];
                        } else {
                            if(numloops != 0) {
                                loop_positions[--numloops] = 0;
                            }
                        }
                        break;

                    // Shortcuts
                    case '_':
                        p = 0;
                        break;
                    case '^':
                        a[p] = 0;
                        break;
                    case '|':
                        {
                            char v[4] = {0};
                            int count = 0;
                            for( c = code[codepos++];
                                 c != '|' && count < 3;
                                 c = code[codepos++], count++
                            ) {
                                if(c >= 48 && c < 58) {
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
                                a[p] += atoi(v);
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
                                    error = 1;
                                    break;
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                    registers[c - 48] = a[p];
                                    break;
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    extregisters[c - 53] = a[p];
                                    break;
                                case '{':
                                    mode = MODE_REG_MANIP;
                                    break;
                                case '+':
                                    registers[1] += a[p];
                                    break;
                                case '-':
                                    registers[1] -= a[p];
                                    break;
                                case '/':
                                    registers[1] /= a[p];
                                    break;
                                case '*':
                                    registers[1] *= a[p];
                                    break;
                                case '%':
                                    registers[1] %= a[p];
                                    break;
                                case '&':
                                    registers[1] &= a[p];
                                    break;
                                case '|':
                                    registers[1] |= a[p];
                                    break;
                                case '^':
                                    registers[1] ^= a[p];
                                    break;
                                case '!':
                                    registers[1] = ~registers[1];
                                    break;
                                default:
                                    printf("Syntax error: Invalid register operation '%c'.\n", c);
                                    error = ERROR_UNKNOWN;
                                    break;
                            }
                        }
                        break;
                    case 's':
                        a[p] ^= registers[1];
                        registers[1] ^= a[p];
                        a[p] ^= registers[1];
                        break;
                    case 'p':
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
                                    printf("%c", registers[c - 48]);
                                    break;
                                case '5':
                                    printf("%x", extregisters[0]);
                                    break;
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    printf("%d", extregisters[c - 53]);
                                    break;
                                default:
                                    printf("Error: Unknown register '%c'.", c);
                                    break;
                            }
                        }
                        break;

                    // Stack
                    case '"':
                        bc_push(&s, a, p);
                        break;

                    case '\'':
                        bc_pop(&s, a, p);
                        break;

                    // Debugging
                    case 'h':
                        printf("0x%x", (a[p] >= 0) ? a[p] : 256 + a[p]);
                        break;
                    case 'd':
                        printf("%d", (a[p] >= 0) ? a[p] : 256 + a[p]);
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
            printf("\n\nNew Stats: %d - %c - %d\n\n", p, a[p], a[p]);
        }
    }
    s.destruct(&s);
    registry.destruct(&registry);
    return error;
}

int
bc_reg_manip(Registry* r, char* manip_command)
{
    int error = 0;
    char mode = MODE_REG_MANIP_SET;
/*    switch(c)
    {
        case '!':
            if(mode == MODE_REG_MANIP_DONE ) {
                printf("Syntax Error: Registry manipulation is already complete!\n");
                error = ERROR_SYNTAX;
            } else if( mode == MODE_REG_MANIP_NOT ) {
                printf("Syntax Error: Already specified not!\n");
                error = ERROR_SYNTAX;
            } else {
                mode = MODE_REG_MANIP_NOT;
            }
            break;
        case '0':
            printf("Error: Can not manipulate a read only register!\n");
            error = ERROR_OOB;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
            if(mode == MODE_REG_MANIP_DONE) {
                printf("Syntax Error: Registry manipulation is already complete!\n");
                error = ERROR_SYNTAX;
            } else if( mode == MODE_REG_MANIP_NOT ) {
                registers[c - 48] = ~registers[c - 48];
            }
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if(mode == MODE_REG_MANIP_DONE) {
                printf("Syntax Error: Registry manipulation is already complete!\n");
                error = ERROR_SYNTAX;
            } else if( mode == MODE_REG_MANIP_NOT ) {
                extregisters[c - 53] = ~extregisters[c - 53];
            }
            break;
        case '}':
           mode = MODE_EXEC;
           break;
        default:
           printf("Syntax Error: Invalid registry manipulation function.\n");
           error = ERROR_SYNTAX;
        break;
    }*/
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
