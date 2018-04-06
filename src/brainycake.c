#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <glib-2.0/glib.h>

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
char *function_plt;

// Initialize the offset as 1, so that we don't match NULL...
int plt_offset = 1;

/**
 * Preprocesses the main code file using bc_include() for includes
 */
int
bc_preprocess(char* mainfile, char** code)
{
    char c = NULL;
    char error = ERROR_NORMAL;

#ifdef TEST
    write_log("Opening file %s...\n", mainfile);
#endif
    FILE* f = fopen(mainfile, "rb");
    if(!f) {
#ifdef TEST
        write_log("Error: ERROR_BADFILE\n");
#endif
        return ERROR_BADFILE;
    }
#ifdef TEST
    write_log("Success!\n");
#endif

    int codepos = 0;
    struct stat filestat;
    stat(mainfile, &filestat);
    int size = filestat.st_size;

#ifdef TEST
    write_log("File is %d bytes long, allocating %d + 1...\n", size, size);
#endif
    *code = calloc(sizeof(char), size+1);
    mode = MODE_PREPROCESS;
#ifdef TEST
    write_log("Done; Mode: %d\n", mode);
#endif

    while( !error && (c = fgetc(f)) != EOF) {
#ifdef TEST
    write_log("Mode: %d; Code Position: %d\n", mode, codepos);
#endif
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
#ifdef TEST
                 write_log("We are in output mode, adding '%c' to code...\n", c);
#endif
                 (*code)[codepos++] = c;
                 if( c == '\\' ) {
#ifdef TEST
                     write_log("That was the end of the output block, setting"
                               " mode back to preprocessing...\n");
#endif
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
#ifdef TEST
                            write_log("Not whitespace/comment or we are in output mode,"
                                      " adding char '%c'to code...\n", c);
#endif
                            (*code)[codepos++] = c;
                        }
                        break;
                }
        }
    }

#ifdef TEST
    write_log("Finished loading code into memory, optimize is set to: %d\n", optimize);
#endif
    if(optimize) {
#ifdef TEST
        write_log("Beginning optimization...\n");
#endif
        bc_optimize(code, codepos);
#ifdef TEST
        write_log("Done.\n");
#endif
    }

    return error;
}

int
bc_include(char* file, char** functions, char** code, int cursize)
{
    FILE* f;
}

void bc_print(Registry* r, Stack* s)
{
#ifdef TEST
    write_log("Got into the built-in print function...\n");
    r->printRegisters(r);
#endif
    char* str = (char*) r->extregisters[0];
    printf("%s", str);
}

void bc_call(GHashTable *function_table, GHashTable *ud_function_table, char* function_name, Registry* r, Stack* s)
{
    // In the case of user defined functions, we'll have bc_call_ud returned to us  We will call that with the
    // Function table and the function name amd it will- figure out what to do from there
    void (*function)(Registry*, Stack*, char*, GHashTable*) = g_hash_table_lookup(function_table, function_name);

#ifdef TEST
    write_log("Attempting to call the built-in function '%s'\n", function_name);
#endif

    if(function) {

#ifdef TEST
        write_log("The function '%s' exists.\n", function_name);
#endif

        // So...in hindsight...by having this wrapper here, we're going to add a ton of unnecessary stack frames
        // Maybe we should change that....
        (*function)(r, s, function_name, ud_function_table);
    } else {
        printf("The function '%s' is not defined!\n", function_name);
    }
}

// In order for this to actually work, we need to make bc_execute abstracted out a bit more....
//   We'll need to be able to parse the program and initialize memory separately otherwise we can't return data
void bc_call_ud(Registry* r, Stack* s, char* function_name, GHashTable *ud_function_table)
{
#ifdef TEST
    write_log("Attempting to call user defined function (%lx) '%s'\n", function_name, function_name);
#endif
    char* code = NULL;
    int function_offset = g_hash_table_lookup(ud_function_table, function_name);
    if(function_offset != NULL) {
        code = function_plt + function_offset;
    }

    if(code != NULL) {
#ifdef TEST
        write_log("It's code (%lx) is: %s\n", code, code);
#endif
//        bc_execute(code);
    }
}

void bc_builtin(GHashTable **function_table)
{
#ifdef TEST
    write_log("Defining the built-in function print as %x in %x\n", bc_print, *function_table);
#endif
    g_hash_table_insert(*function_table, "print", bc_print);
}

void init_memory(bcMemory* m, char* code)
{
    bcMemory mem = *m;
    GHashTable *function_table = g_hash_table_new(g_str_hash, g_str_equal);
    bc_builtin(&function_table);
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
    char step_by_step = 0;
}

// bc_execute - Execute brainycake code
int
bc_execute(char* code)
{
    // Define/initialize basic variables
#ifdef TEST
     write_log("MAX_LOOPS: %d\n");
     write_log("INIT_CELL_COUNT: %d\n", INIT_CELL_COUNT);
#endif

    GHashTable *function_table = g_hash_table_new(g_str_hash, g_str_equal);
    bc_builtin(&function_table);
    GHashTable *ud_function_table = g_hash_table_new(g_str_hash, g_str_equal);
    printf("Code at: %x\n", code);
    int codelen = strlen(code);
    char c;
    char* loop_positions[MAX_LOOPS] = {0};
    unsigned int numloops = 0;
    unsigned int numloops_orig = 0;
    function_plt = (char*) calloc(sizeof(char), 255);

#ifdef TEST
    write_log("Allocating space for tape...\n");
#endif
    signed char* tape = calloc(sizeof(char), INIT_CELL_COUNT);
#ifdef TEST
    write_log("Done! Address: %x\n", tape);
#endif

    signed char** a;
    int curCellCount = INIT_CELL_COUNT;
    int error = ERROR_NORMAL;
    signed int* regArgv;
    int regArgc = 0;
    srand(time(NULL));
    char** ip;
    char step_by_step = 0;

    // Set our mode to execution
    mode = MODE_EXEC;

    // Initialize memory objects
#ifdef TEST
    write_log("Creating a registry object; code at %x, tape at %x...\n", code, tape);
#endif
    Registry registry;
    Registry_construct(&registry, (int*) 0, (int*) 0, (int*) code, (int*)tape);
#ifdef TEST
    write_log("Done! Address: %x\n", &registry);
#endif

#ifdef TEST
    write_log("Creating stack object...STACK_PTR = %d; BASE_PTR = %d; extregisters = %x...\n",
              STACK_PTR, BASE_PTR, &registry.extregisters);
#endif
    Stack s;
    Stack_construct(&s, MAX_STACK_HEIGHT, &registry.extregisters[STACK_PTR], &registry.extregisters[BASE_PTR]);
#ifdef TEST
    write_log("Done! Address: %lx\n", &s);
#endif

    // Set the tape pointer to point to the register
#ifdef TEST
    write_log("Setting the tape pointer to point to the register at address: %x\n",
              &(registry.extregisters[TAPE_PTR]));
#endif
    a = (char**)(&(registry.extregisters[TAPE_PTR]));

    // Set the instruction pointer to point to the register
#ifdef TEST
    write_log("Setting the instruction pointer to point to the register at address: %x\n",
              &(registry.extregisters[INSTRUCTION_PTR]));
#endif
    ip = (char**) (&(registry.extregisters[INSTRUCTION_PTR]));


    // Set c to the value at ip, run while it is not at the end of the code
    //  After each loop, increase the instruction pointer by one and set c again
#ifdef TEST
    write_log("Instruction pointer started at: %x\n", *ip);
    registry.printRegisters(&registry);
#endif
    for( c = **ip ; (*ip - code) <= codelen && (!error); *ip = *ip+1, c = **ip) {
#ifdef TEST
        write_log("Char: %c; IP: %x; Code: %x; Codelen: %d\n", c, (*ip - code), code, codelen);
#endif

        // If we're running in verbose or superverbose mode
        if(verbose + superverbose && !step_by_step) {
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
                switch(c)
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

                                // If it is a '2'
                                case '2':
                                     // Multiply register 5 by register 5
                                     bc_reg_manip(&registry, &s, "5*5");
                                     break;

                                // If it is a '3'
                                case '3':
                                     {
                                         signed int v1 = 0;
                                         signed int v2 = 0;

                                         // Preserve the original value of register 5
                                         registry.getRegisterValue(&registry, '5', &v1);

                                         // Multiply register 5 by register 5 
                                         bc_reg_manip(&registry, &s, "5*5");

                                         // Get the squared value of register 5
                                         registry.getRegisterValue(&registry, '5', &v2);

                                         // Set register 5 to the squared value times the original value
                                         registry.setRegister(&registry, '5', v1*v2);
                                     }
                                     break;

                                // Registers
                                // If it is a $
                                case '$':
                                    // Increment the instruction pointer and get the next char
                                    *ip = *ip + 1;
                                    c = **ip;

                                    // If we're at the end of the filee
                                    if( c == EOF ) {
                                        // Error out
                                        printf("Error: Scanned to end of file when attempting to identify a register.");
                                    }
                                    // Otherwise
                                    else {
                                        // Determine what to do based on the char
                                        switch(c)
                                        {
                                            // If it's a 0
                                            case '0':
                                                // NOP
                                                break;
                                            // If it's 1-9
                                            case '1':
                                            case '2':
                                            case '3':
                                            case '4':
                                            case '5':
                                            case '6':
                                            case '7':
                                            case '8':
                                            case '9':
                                                // Set the specified register to the value at the tape pointer
                                                registry.setRegister(&registry, c, (**a));
                                                break;
                                            // If it's a {
                                            case '{':
                                                // Set us to register manipulation mode
                                                mode = MODE_REG_MANIP;
                                                break;
                                            // If it's +, -, /, *, %, &, |, ^, or !
                                            case '+':
                                            case '-':
                                            case '/':
                                            case '*':
                                            case '%':
                                            case '&':
                                            case '|':
                                            case '^':
                                            case '!':
                                                // Perform the operation with a default register
                                                regArgv = (signed int*) 0;
                                                regArgc = 0;
                                                registry.performOperation(&registry, c, regArgv, regArgc);
                                                break;
                                            // Otherwise
                                            default:
                                                // Set error to ERROR_BADOP
                                                error = ERROR_BADOP;
                                                break;
                                        }
                                    }
                                    break;

                                // If it is a s
                                case 's':
                                    // Perform a switch operation with the default registers
                                    regArgv = (signed int*) 0;
                                    regArgc = 0;
                                    registry.performOperation(&registry, 's', regArgv, regArgc);
                                    break;

                                // If it is a p
                                case 'p':
                                    {
                                        int val;
                                        // Increement the instruction pointer and get the next char
                                        *ip = *ip + 1;
                                        c = **ip;

                                        // If we're at the end of the file
                                        if( c == EOF ) {
                                            // Error out
                                            printf("Error: Scanned to end of file when attempting to identify register to print.");
                                        }
                                        // Otherwise
                                        else {
                                            // Determine what to do baseed on the char
                                            switch(c)
                                            {
                                                // If it's a 0
                                                case '0':
                                                    // Output NULL
                                                    printf("NULL");
                                                    break;
                                                // If it's 1-4
                                                case '1':
                                                case '2':
                                                case '3':
                                                case '4':
                                                    // Output the value of the specified register as a char
                                                    error = registry.getRegisterValue(&registry, c, &val);
                                                    printf("%c", val);
                                                    break;
                                                case '5':
                                                case '6':
                                                case '7':
                                                case '8':
                                                // If it's 5-9
                                                case '9':
                                                    // Output the specified register as an integer
                                                    error = registry.getRegisterValue(&registry, c, &val);
                                                    printf("%d", val);
                                                    break;

                                                // Otherwise
                                                default:
                                                    // Set error to ERROR_UNKREG
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
                                    // If the current cell on the tape is 0
                                    if(!(**a)) {
                                        // Set error to ERROR_PROG
                                        error = ERROR_PROG;
                                    }
                                    break;

                                // If it is a ?
                                case '?':
                                    {
                                        // Output debug information
                                        bc_debug(&s, &registry, tape, curCellCount);
                                        printf("----- Press Enter to Continue -----\n");
                                        char breakpoint_input;

                                        // Wait for the user to hit enter to continue
                                        do {
                                            breakpoint_input = getchar();
                                            printf("%d\n", breakpoint_input);
                                        } while(breakpoint_input != '\n');

                                        // If gdb is enabled
                                        if(gdb) {
                                            // Emit the SIGINT signal
                                            raise(SIGINT);
                                        }
                                    }
                                    break;

                                // If it is a !
                                case '!':
                                    step_by_step = !step_by_step;
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

                                // If it is a j
                                case 'j':
                                    {
                                        // Keep track of the maximum number of digits
                                        int max_digits = ceil(log10(codelen));

                                        // Hold the jump value
                                        int jv = 0;

                                        // Allocate an array to hold the highest number of chars rquired by code length
                                        char v[max_digits + 2];

                                        // Increment instruction pointer and retrieve the next char
                                        *ip = *ip + 1;
                                        c = **ip;

                                        int i = 0;

                                        // Determine what to do based on the char
                                        switch(c)
                                        {
                                            // If  it is a {, then we are doing an absolute jump
                                            case '{':
                                                // Retrieve the next chars until max digits, EOF, or }
                                                for( *ip = *ip + 1, c = **ip; **ip != '}' && **ip != EOF && **ip < (code + codelen) && i < max_digits; *ip = *ip + 1, c = **ip, i++) {
                                                    // If the char is not numeric, set error condition and break out of loop
                                                    if(**ip > 57 || **ip < 48) {
                                                        error = ERROR_UNKNOWN;
                                                        break;
                                                    }
                                                    // Otherwise add it to the array
                                                    v[i] = **ip;
                                                }
                                                // Until we fill up the remaining cells, set to null
                                                while(++i <= max_digits) v[i] = '\0';

                                                // If there's no error condition
                                                if(!error) {
                                                    // Set ip to code + value - 1 (to account for 0 index)
                                                    *ip = code + atoi(v) - 1;
                                                }
                                                break;

                                            // If it is a [, then we are doing a relative jump
                                            case '[':
                                                for( *ip = *ip + 1, c = **ip; **ip != ']' && **ip != EOF && **ip < (code + codelen) && i < max_digits; *ip = *ip + 1, c = **ip, i++) {
                                                    if(**ip != '-' && (**ip < 48 || **ip > 57)) {
                                                        error = ERROR_UNKNOWN;
                                                        break;
                                                    }
                                                    v[i] = **ip;
                                                }
                                                while(++i <= max_digits) v[i] = '\0';
                                                if(!error) {
                                                    *ip = *ip + atoi(v);
                                                }
                                                break;
                                        }
                                     }
                                     break;
                                case '`':
                                    {
                                        gboolean success = 0;
                                        char function[50] = { '\0' };
                                        unsigned short int i = 0;
                                        *ip = *ip + 1;
                                        for( ; i < 50; i++) {
                                            if(**ip == '`') {
                                                bc_call(function_table, ud_function_table, function, &registry, &s);
                                                break;
                                            } else {
                                                function[i] = **ip;
                                            }
                                            *ip = *ip + 1;
                                        }
                                     }
                                    break;

                                case '(':
#ifdef TEST
                                    write_log("Defining a function\n");
#endif
                                    {
                                        gboolean success = 0;
                                        char function[50] = { '\0' };
                                        unsigned short int i = 0;
                                        *ip = *ip + 1;
                                        for( ; i < 50; i++) {
                                            if(**ip == ':') {
                                                g_hash_table_insert(function_table, function, bc_call_ud);
                                                *ip = *ip + 1;
                                                break;
                                            } else {
                                                function[i] = **ip;
                                            }
                                            *ip = *ip + 1;
                                        }
#ifdef TEST
                                        write_log("The function is called '%s'\n", function);
#endif
                                        int function_offset = plt_offset;
                                        while(**ip != ')' && **ip != EOF) {
#ifdef TEST
                                            write_log("Attempting to assign '%c' to %lx\n", **ip, function_plt + plt_offset);
#endif
                                            function_plt[plt_offset++] = **ip;
                                            *ip = *ip + 1; // Go to the end
                                        }

                                        // Increment once more for a null byte
                                        plt_offset++;
#ifdef TEST
                                        write_log("It's code (at %lx) is '%s'\n", function_plt, (char*) (function_plt + function_offset));
#endif
                                        g_hash_table_insert(ud_function_table, function, function_offset);
                                     }
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


        if(step_by_step) {
            {
                // Output debug information
                bc_debug(&s, &registry, tape, curCellCount);
                printf("----- Press Enter to Continue -----\n");
                char breakpoint_input;

                // Wait for the user to hit enter to continue
                do {
                    breakpoint_input = getchar();
                    printf("%d\n", breakpoint_input);
                } while(breakpoint_input != '\n');
            }
        }

        // If we're in verbose or superverbose mode
        if(verbose + superverbose && !step_by_step) {
            // Output debug information
            bc_debug(&s, &registry, tape, curCellCount);
        }
    }

    // If we're in debug mode and not in verbose or superverbose mode 
    if(debug && !verbose && !superverbose && !step_by_step) {
        // Output debug information
        bc_debug(&s, &registry, tape, curCellCount);
    }

    // Free memory allocated for the stack
    s.destruct(&s);

    // Free memory allocated for the registry
    registry.destruct(&registry);

    // Free memory allocated for the tape
    free(tape);

    // Free our function plt
    free(function_plt);

    // Destroy the function table
    g_hash_table_destroy(function_table);

    // Return our error code
    return error;
}

// bc_reg_manip
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
        case '@':
        case '^':
            argc = 2;
            if(reg2 == '\0') reg2 = 't';
            if(reg1 == '\0') reg1 = 't';
            if(reg1 == '\0' || !((reg1 > 48 && reg1 < 58) || reg1 == '&' || reg1 == '*' || reg1 == 'i' || reg1 == 't' || reg1 == 's' || reg1 == 'b')) return ERROR_UNKREG;
            if(reg2 == '\0' || !((reg2 > 48 && reg2 < 58) || reg2 == '&' || reg2 == '*' || reg2 == 'i' || reg2 == 't' || reg2 == 's' || reg2 == 'b')) return ERROR_UNKREG;
            if(reg1 == '0' || reg2 == '0') return ERROR_RO;
            break;
        default:
            return ERROR_BADOP;
    }

    argv[0] = reg1;
    argv[1] = reg2;

    return (*r).performOperation(r, op, argv, argc);
}

// bc_push
void
bc_push(Stack* s, signed char* a, int p)
{
    int v = 0;
    int x = sizeof(int) - 1;
    for( ; x >= 0 ; x-- ) {
        // @note Figure out why the & (0xff << (x*8)) is needed, this is a possible explanation (See #15)
        // The resulting value on the right side is being silently converted
        //  to an int, due to this, the | operation being done affects more
        //  just a single byte of v.  By &'ing prior to doing the |, we zero
        //  out the rest of the value....this is my hypothesis, will be
        //  testing
        v |= (a[p++] << (x * 8)) & (0xff << (x*8));
    }
    s->push(s, v);
}

// bc_pop
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

// bc_debug
void bc_debug(Stack* s, Registry* r, signed char* tape, int curCellCount)
{
    int curCell = 0;
    printf("=======    Tape   ======\n");
    for( ; curCell < curCellCount ; curCell++) {
        if(tape[curCell] != (char) 0) {
            // Format the current cell with the proper number of columns for the number of digits
            printf("Cell #%*d: %02x\n", 0 - (int)ceil(log10(curCellCount)), curCell + 1, tape[curCell]);
        }
    }
    curCell = (signed char*) r->extregisters[TAPE_PTR] - tape;
    printf("Current Cell (%d/%d): %02x\n", curCell + 1, curCellCount, tape[curCell]);
    printf("======= Registers ======\n");
    printf("|TP - %d|BP - %d|SP - %d|IP - %d|PP - %d|\n", TAPE_PTR, BASE_PTR, STACK_PTR, INSTRUCTION_PTR, POINTER_PTR);
    r->printRegisters(r);
    printf("========   Stack   =====\n");
    s->printStack(s);
    r->translateRegister((void*) r, 'b');
}
