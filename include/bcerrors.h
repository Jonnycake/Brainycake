// Error codes
#define ERROR_NORMAL  0  // No error
#define ERROR_SYNTAX  1  // Syntax errors
#define ERROR_OOB     2  // Out of bounds - trying to push onto full stack, etc.
#define ERROR_RO      3  // Read-only - Trying to write to $0
#define ERROR_UNDEF   4  // Undefined function
#define ERROR_PROG    5  // Program set error condition
#define ERROR_BADOP   6  // Bad registry operation
#define ERROR_DIVZERO 7  // Divide by zero error
#define ERROR_LOOP    8  // Too many nested loops (or an infinite loop)
#define ERROR_CORRUPT 9  // Tape or stack has been corrupted
#define ERROR_MEMORY  10 // Memory limit has been reached
#define ERROR_UNKREG  11 // Unknown register
#define ERROR_UNKNOWN 12 // All other error conditions
