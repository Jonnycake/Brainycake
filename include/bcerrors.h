// Error codes
#define ERROR_SYNTAX  1 // Syntax errors
#define ERROR_OOB     2 // Out of bounds - trying to push onto full stack, etc.
#define ERROR_RO      3 // Read-only - Trying to write to $0
#define ERROR_UNDEF   4 // Undefined function
#define ERROR_PROG    5 // Program set error condition
#define ERROR_UNKNOWN 6 // All other error conditions
