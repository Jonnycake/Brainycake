#ifndef BRAINYCAKE
    #include <bcerrors.h>
    #include <stack.h>
    #include <registry.h>
    #include <glib-2.0/glib.h>

    // Don't load this again
    #define BRAINYCAKE 1

    // Memory settings
    #define INIT_CELL_COUNT   64        // Number of cells to initalize for basic array
    #define MAX_CELL_COUNT    256       // Maximum number of cells to allow
    #define MAX_STACK_HEIGHT  1024      // Maximum height stack can get to
    #define STACK_WIDTH       4         // Number of bytes per level of the stack
    #define MAX_LOOPS         32        // Maximum number of nested loops
    #define MAX_FILENAME      255       // Maximum length of a filename

    // Parsing modes
    #define MODE_START          0       // Very beginning of the process
    #define MODE_PREPROCESS     1       // Preprocessing optimizations
    #define MODE_JUMP_ABS       2       // Absolute jump
    #define MODE_JUMP_REL       3
    #define MODE_COMMENT_SL     4
    #define MODE_COMMENT_ML     5
    #define MODE_EXEC           6
    #define MODE_OUTPUT         7
    #define MODE_FUNC_INIT      8
    #define MODE_FUNC_DEF       9
    #define MODE_INCLUDE        10
    #define MODE_REG_MANIP      11
    #define MODE_COND_EVAL      12
    #define MODE_COLLAPSE_PLUS  13
    #define MODE_COLLAPSE_MINUS 14
    #define MODE_EXIT           15
    #define MODE_OPTIMIZE       16
    #define MODE_ERROR          17

    /**
     * The structure used for memory initialization
     */
     typedef struct {
        GHashTable *function_table;
        int codelen;
        char c;
        char* loop_positions[MAX_LOOPS];
        unsigned int numloops;
        unsigned int numloops_orig;
        signed char* tape;
        signed char** a;
        int curCellCount;
        int error;
        signed int* regArgv;
        int regArgc;
        char** ip;
        char step_by_step7;
     } bcMemory;

    /**
     * bc_preprocess will take in mainfile, look for syntax errors,
     * remove whitespace and comments, process include files, and
     * place the resulting brainycake code into the char* pointed
     * to by the second argument.  It will return the integer
     * error condition.
     */
    int bc_preprocess(char* mainfile, char** code);

    /**
     * bc_include will take in the file, locate the functions listed,
     * place them at the beginning of code (ensuring that it has enough
     * space), and return the new size of the code array
     */
    int bc_include(char* file, char** functions, char** code, int cursize);

    /**
     * bc_execute will take in code and interpret and execute it
     * and return the error condition.
     */
    int bc_execute(char* code);

    /**
     * Brainycake Stack functions
     */
    void bc_push(Stack* s, signed char* a, int p);
    void bc_pop(Stack* s, signed char* a, int p);

    /**
     * Brainycake Registry Manipulation
     */
     int bc_reg_manip(Registry* r, Stack* s, char* manip_command);

     void bc_debug(Stack* s, Registry* r, signed char* tape, int curCellCount);



     int bc_optimize(char** code, int codepos);
#endif

