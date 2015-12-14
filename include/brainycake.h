#ifndef BRAINYCAKE
    #include <bcerrors.h>
    #include <stack.h>
    #include <registry.h>
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
    #define MODE_START          0
    #define MODE_PREPROCESS     1
    #define MODE_JUMP_ABS       2
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
    #define MODE_REG_MANIP_DONE 13
    #define MODE_REG_MANIP_NOT  14
    #define MODE_REG_MANIP_ADD  15
    #define MODE_REG_MANIP_SUB  16
    #define MODE_REG_MANIP_MUL  17
    #define MODE_REG_MANIP_DIV  18
    #define MODE_REG_MANIP_MOD  19
    #define MODE_REG_MANIP_SET  20

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
     * bc_jump will move the code position depending on the current
     * jump mode (relative or absolute)
     */
    void bc_jump(char* code, int* codepos);

    /**
     * Brainycake Stack functions
     */
    void bc_push(Stack* s, signed char* a, int p);
    void bc_pop(Stack* s, signed char* a, int p);

    /**
     * Brainycake Registry Manipulation
     */
     int bc_reg_manip(Registry* r, char* manip_command);

     void bc_debug(Stack* s, Registry* r, signed char* tape, int curCellCount);
#endif
