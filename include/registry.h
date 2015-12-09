#define CHAR_REG_COUNT  5
#define EXT_REG_COUNT   9
#define BASE_PTR        EXT_REG_COUNT - 4
#define STACK_PTR       EXT_REG_COUNT - 3
#define INSTRUCTION_PTR EXT_REG_COUNT - 2
#define TAPE_PTR        EXT_REG_COUNT - 1

typedef struct {
     unsigned char* registers;
     char* extregisters;
     int (*setRegister)(void*, char, int);
     int (*performOperation)(void*, char, int*, int);
     int (*getRegisterValue)(void*, char, int*);
     int* (*translateRegister)(void*, int);
     int (*switchRegisters)(void*, int* reg1, int* reg2);
     void (*construct)(void*);
     void (*destruct)(void*);
} Registry;

// Constructor/Destructor
void Registry_construct(void* r);
void Registry_destruct(void* r);

// Registry object methods
int Registry_setRegister(void* r, char reg, int val);
int Registry_performOperation(void* r, char op, int* argv, int argc);
int Registry_getRegisterValue(void* r, char reg, int* target);
int* Registry_translateRegister(void* r, int regNum);

// Utility functions
int Registry_switchRegisters(void* r, int* reg1, int* reg2);
int Registry_doArithmetic(char op, int* r1, int* r2);
int Registry_doLogic(char op, int* r1, int* r2);
