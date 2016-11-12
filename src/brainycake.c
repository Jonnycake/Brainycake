#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>

#define MIN_LOAD 1
#include <brainycake.h>


// Current processing mode
char mode = MODE_START;

// Error reporting modes
char quiet = 0;
char verbose = 0;
char debug = 0;
char superverbose = 0;
char traditional = 0;
char optimize = 0;
char gdb = 0;

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
    *code = calloc(sizeof(char), size+1);
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
    if(optimize) {
        bc_optimize(code, codepos);
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
    // Define/initialize basic variables
    int codelen = strlen(code);
    char c;
    char* loop_positions[MAX_LOOPS] = {0};
    unsigned int numloops = 0;
    unsigned int numloops_orig = 0;
    signed char* tape = calloc(sizeof(char), INIT_CELL_COUNT);
    signed char** a;
    int curCellCount = INIT_CELL_COUNT;
    int error = ERROR_NORMAL;
    signed int* regArgv;
    int regArgc = 0;
    srand(time(NULL));
    char** ip;

    // Set our mode to execution
    mode = MODE_EXEC;

    // Initialize memory objects
    Registry registry;
    Registry_construct(&registry, (int*) 0, (int*) 0, (int*) code, (int*)tape);
    Stack s;
    Stack_construct(&s, MAX_STACK_HEIGHT, &registry.extregisters[STACK_PTR], &registry.extregisters[BASE_PTR]);

    // Set the tape pointer to point to the register
    a = (char**)(&(registry.extregisters[TAPE_PTR]));

    // Set the instruction pointer to point to the register
    ip = (char**) (&(registry.extregisters[INSTRUCTION_PTR]));


    // Set c to the value at ip, run while it is not at the end of the code
    //  After each loop, increase the instruction pointer by one and set c again
    for( c = **ip ; (*ip - code) <= codelen; *ip = *ip+1, c = **ip) {
        // If we're running in verbose or superverbose mode
        if(verbose + superverbose) {
            // Output debug information
            bc_debug(&s, &registry, tape, curCellCount);
        }

        // Determine what to do based on mode
        // @todo When determining modes, we should check to make sure we're not in traditional mode
        //       we can theoretically assume not, but maybe we should be positive
        switch(mode)
        {
            // If we're in mode output
            case MODE_OUTPUT:
                // Unless output mode is being deactivated, output the character
                if( c == '\\' ) {
                    mode = MODE_EXEC;
                } else {
                    putchar(c);
                }
                break;

            // If we're performing a registry operation
            case MODE_REG_MANIP:
                {
                    char reg_cmd[3] = {'\0'};
                    int  i = 0;
                    *ip = *ip - 1;

                    // Set ip to the next char, run while the char is not } and we haven't gone past 3 chars
                    //  After each loop, move ip forward one and increase the index in the reg command
                    for( *ip = *ip + 1; (c = **ip) != '}' && i < 3 ; *ip = *ip+1, i++) {
                        reg_cmd[i] = c;
                    }

                    // If the last char is not a }, there has been a syntax error
                    if(c != '}') {
                        error = ERROR_SYNTAX;
                    }
                    // Otherwise perform the registry operation
                    else {
                        error = bc_reg_manip(&registry, &s, reg_cmd);
                    }
                    mode = MODE_EXEC;
                }
                break;

            // If we're executing commands
            case MODE_EXEC:
                // Determine what to do based on the char
                switch((c)
                {
                    // Traditional brainfuck
                    // If it is a >
                    case '>':
                        // Move right on the tape and check if it's past the end of the tape
                        //  If so,
                        if((++(*a) - tape) >= curCellCount) {
                            // If the current cell count plus the cell count to initialize on each
                            //   cycle is greater than the maximum cell count, wrap back around
                            if((curCellCount + INIT_CELL_COUNT) > MAX_CELL_COUNT) {
                                *a = tape;
                            }
                            // Otherwise, add more cells to the tape
                            else {
                                curCellCount += INIT_CELL_COUNT;
                                tape = realloc(tape, curCellCount * sizeof(char));
                            }
                        }
                        break;

                    // If it is a <
                    case '<':
                        // Move left on the tape, if it's before the beginning of the tape
                        if(--(*a) < tape) {
                            // Move to the end of the tape
                            *a = tape + curCellCount - 1;
                        }
                        break;

                    // If it is a .
                    case '.':
                        // Output the char at the tape pointer
                        putchar(**a);
                        break;

                    // If it is a ,
                    case ',':
                        // Set the value at the tape pointer to user input
                        **a = getchar();
                        break;

                    // If it is a +
                    case '+':
                        // Increment the value at the tape pointer
                        (**a)++;
                        break;

                    // If it is a -
                    case '-':
                        // Decrement the value at the tape pointer
                        (**a)--;
                        break;

                    // If it s a [
                    case '[':
                        // Increment numloops
                        numloops++;

                        // If the value at the tape pointer is nonzero
                        if((**a)) {
                            // Store the position of the loop as instruction pointer plus 1
                            loop_positions[numloops - 1] = *ip;
                        }
                        // Otherwise
                        else {
                            // Set numloops_orig to the current numloops minus 1
                            numloops_orig = numloops - 1;

                            // While we have not escape this loop nor hit the end of the file
                            do {
                                // Increment the instruction pointer
                                *ip = *ip + 1;

                                // Retrieve the next char
                                c = **ip;

                                // If the char is a [
                                if(c == '[') {
                                    // Increment numloops
                                    numloops++;
                                }
                                // Otherwise, if the char is a ]
                                else if(c == ']') {
                                    // Decrement numloops
                                    numloops--;
                                }
                                // Otherwise, do nothing
                            } while(numloops != numloops_orig && c != EOF);
                        }
                        break;

                    // If it is a ]
                    case ']':
                        // If we're not currently in a loop
                        if(!numloops) {
                            // Set error to ERROR_SYNTAX
                            error = ERROR_SYNTAX;
                        }

                        // If the value at the tape pointer is nonzero
                        if((**a)) {
                            // Jump back to the start of this loop
                            *ip = loop_positions[numloops - 1];
                        }
                        // Otherwise
                        else {
                            // Decrement numloops
                            numloops--;
                        }
                        break;

                    // BEGIN NON-TRADITIONAL BRAINFUCK //
                    default:
                        // If we are not operating in traditional mode
                        if(!traditional) {
                            // Determine what to do based on the char
                            switch(c) {
                                // Shortcuts
                                // If it is a _
                                case '_':
                                    // Move to the beginning of the tape
                                    *a = tape;
                                    break;

                                // If it is a ^
                                case '^':
                                    // Set the value at the tape pointer to 0
                                    (**a) = 0;
                                    break;

                                // If it is a |
                                case '|':
                                    {
                                        // Initialize an array to hold the string representation of the number
                                        //   Maximum size, is 5 (3 chars for the number, 1 char for null, 1 char for negative sign
                                        char v[5] = {0};
                                        int i = 0;
                                        char neg = 0;
                                        char v_val;

                                        // Increment the instruction pointer to be the start of the value
                                        for( *ip = *ip + 1 ; *ip < (code + codelen) && **ip != '|' && i < 4; i++, *ip = *ip + 1) {
                                            c = **ip;
                                            if(!neg && i == 3) {
                                                error = ERROR_UNKNOWN;
                                                break;
                                            }
                                            if((c != '-' || i > 0) && (c > 57 || c < 48)) {
                                                error = ERROR_UNKNOWN;
                                                break;
                                            }

                                            // If c is a minus sign and we're at the first char
                                            if(c == '-' && i == 0) {
                                                neg = 1;
                                            }

                                            v[i] = c;
                                        }
                                        if(!error) {
                                            v_val = (char) atoi(v);
                                            **a += v_val;
                                        }
                                    }
                                    break;

                                // If it is a '\'
                                case '\\':
                                    // Set us to output mode
                                    mode = MODE_OUTPUT;
                                    break;

                                // Registers
                                // If it is a $
                                case '$':
                                    *ip = *ip + 1;
                                    c = **ip;
                                    if( c == EOF ) {
                                        printf("Error: Scanned to end of file when attempting to identify a register.");
                                    } else {
                                        switch(c)
                                        {
                                            case '0':
                                                printf("Warning: Attempted to write to read-only register ($0).");
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
                                                regArgv = (signed int*) 0;
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

                                // If it is a s
                                case 's':
                                    regArgc = 0;
                                    registry.performOperation(&registry, 's', regArgv, regArgc);
                                    break;

                                // If it is a p
                                case 'p':
                                    {
                                     int val;
                                     *ip = *ip + 1;
                                     c = **ip;
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
                                // If it is a "
                                case '"':
                                    // Push the value at the tape pointer onto  the stack
                                    bc_push(&s, tape, (*a) - tape);
                                    break;

                                // If it is a '
                                case '\'':
                                    // Pop a value off the stack onto the tape
                                    bc_pop(&s, tape, (*a) - tape);
                                    break;

                                // Debugging
                                // If it is a h
                                case 'h':
                                    // Print the value on the tape in hex mode
                                    printf("0x%x", ((**a) >= 0) ? (**a) : 256 + (**a));
                                    break;

                                // If it is a d
                                case 'd':
                                    // Print the value on the tape as an int
                                    printf("%d", ((**a) >= 0) ? (**a) : 256 + (**a));
                                    break;

                                // If it is a v
                                case 'v':
                                    // turn verbose mode on
                                    verbose = (verbose) ? 0 : 1;
                                    break;

                                // If it is a @
                                case '@':
                                    // Set error to ERROR_PROG
                                    error = ERROR_PROG;
                                    break;

                                // If it is a ?
                                case '?':
                                    {
                                        bc_debug(&s, &registry, tape, curCellCount);
                                        printf("----- Press Enter to Continue -----\n");
                                        char breakpoint_input;
                                        do {
                                            breakpoint_input = getchar();
                                            printf("%d\n", breakpoint_input);
                                        } while(breakpoint_input != '\n');
                                        if(gdb) {
                                            raise(SIGINT);
                                        }
                                    }

                                // If it is a !
                                case '!':
                                    // @todo Implement
                                    break;

                                // Extras
                                // If it is a ~
                                case '~':
                                    // Perform a bitwise not on the value at the tape pointer
                                    **a = ~(**a);
                                    break;

                                // If it is an 8
                                case '8':
                                    // Use the not value of the array pointer as a seed for randomness
                                    srand(~((int)**a));
                                    break;

                                // If it is a *
                                case '*':
                                    // Set the value at the array pointer to a random char
                                    **a = (char) rand();
                                    break;

                                // OS Dependent
                                // If it is a c
                                case 'c':
                                    #ifdef WINDOWS
                                    // If we're compiled for windows
                                      // execute cls
                                      system("cls");
                                    #else
                                    // Otherwise
                                      // execute clear
                                      system("clear");
                                    #endif
                                    break;
                        }

                   }
                   break;
                }
                // END NON-TRADITIONAL BRAINFUCK //
                break;

            // If we're in exit mode
            case MODE_EXIT:
                 // Do nothing
                 break;
        }

        // If we're in verbose or superverbose mode
        if(verbose + superverbose) {
            // Output debug information
            bc_debug(&s, &registry, tape, curCellCount);
        }
    }

    // If we're in debug mode and not in verbose or superverbose mode 
    if(debug && !verbose && !superverbose) {
        // Output debug information
        bc_debug(&s, &registry, tape, curCellCount);
    }

    // Free memory allocated for the stack
    s.destruct(&s);

    // Free memory allocated for the registry
    registry.destruct(&registry);

    // Free memory allocated for the tape
    free(tape);

    // Return our error code
    return error;
}

int
bc_reg_manip(Registry* r, Stack* s, char* manip_command)
{
    signed int argv[3] = {0, 0};
    int argc = 0;
    char op, reg1, reg2 = '\0';

    // Parse out the information we need
    switch(manip_command[0])
    {
        case '!':
        case '\'':
        case '"':
            op = manip_command[0];
            reg1 = manip_command[1];
            break;
        case ',':
            op = manip_command[0];
            reg1 = '\0';
            reg2 = manip_command[1];
            break;
        default:
            reg1 = manip_command[0];
            op = manip_command[1];
            reg2 = manip_command[2];
            break;
    }


    // Execute the operation
    switch(op)
    {
        case '\'':
            if(reg1 == '\0' || !((reg1 > 48 && reg1 < 58) || reg1 == 'i' || reg1 == 't' || reg1 == 's' || reg1 == 'b')) return ERROR_UNKREG;
            bc_pop(s, (char*) (*r).translateRegister(r, reg1), 0);
            break;
        case '"':
            if(reg1 == '\0' || !((reg1 > 48 && reg1 < 58) || reg1 == 'i' || reg1 == 't' || reg1 == 's' || reg1 == 'b')) return ERROR_UNKREG;
            bc_push(s, (signed char*) (*r).translateRegister(r, reg1), 0);
            break;
        case '!':
            if(reg2 != '\0') return ERROR_BADOP;
            if(reg1 == '\0' || !((reg1 > 48 && reg1 < 58) || reg1 == 'i' || reg1 == 't' || reg1 == 's' || reg1 == 'b')) return ERROR_UNKREG;
            if(reg1 == '0') return ERROR_RO;
            argc = 1;
            break;
        case ',':
        case '*':
        case '/':
        case '+':
        case '-':
        case '%':
        case '|':
        case '^':
            argc = 2;
            if(reg2 == '\0') reg2 = 't';
            if(reg1 == '\0') reg1 = 't';
            if(reg1 == '\0' || !((reg1 > 48 && reg1 < 58) || reg1 == 'i' || reg1 == 't' || reg1 == 's' || reg1 == 'b' || reg1 == '@')) return ERROR_UNKREG;
            if(reg2 == '\0' || !((reg2 > 48 && reg2 < 58) || reg2 == 'i' || reg2 == 't' || reg2 == 's' || reg2 == 'b' || reg2 == '@')) return ERROR_UNKREG;
            if(reg1 == '0' || reg2 == '0') return ERROR_RO;
            break;
        default:
            return ERROR_BADOP;
    }

    argv[0] = reg1;
    argv[1] = reg2;

    return (*r).performOperation(r, op, argv, argc);
}

void
bc_jump(char* code, int* codepos)
{
}

void
bc_push(Stack* s, signed char* a, int p)
{
    int v = 0;
    int x = sizeof(int) - 1;
    for( ; x >= 0 ; x-- ) {
        v |= (a[p++] << (x * 8)) & (0xff << (x*8));
    }
    s->push(s, v);
}

void bc_pop(Stack* s, signed char* a, int p)
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
            // @todo Figure out why I'm doing ceil and log10 on curCellCount....I don't get it o.O
            printf("Cell #%*d: %02x\n", 0 - (int)ceil(log10(curCellCount)), curCell + 1, tape[curCell]);
        }
    }
    curCell = (signed char*) r->extregisters[TAPE_PTR] - tape;
    printf("Current Cell (%d/%d): %02x\n", curCell + 1, curCellCount, tape[curCell]);
    printf("======= Registers ======\n");
    r->printRegisters(r);
    printf("========   Stack   =====\n");
    s->printStack(s);
    r->translateRegister((void*) r, 'b');
}
