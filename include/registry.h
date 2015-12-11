#define CHAR_REG_COUNT  5
#define EXT_REG_COUNT   9
#define BASE_PTR        EXT_REG_COUNT - 4
#define STACK_PTR       EXT_REG_COUNT - 3
#define INSTRUCTION_PTR EXT_REG_COUNT - 2
#define TAPE_PTR        EXT_REG_COUNT - 1

typedef struct {
     unsigned char* registers;
     signed int* extregisters;
     int (*setRegister)(void*, char, int);
     int (*performOperation)(void*, char, signed int*, int);
     int (*getRegisterValue)(void*, char, signed int*);
     signed int* (*translateRegister)(void*, int);
     int (*switchRegisters)(void*, signed int* reg1, signed int* reg2);
     void (*construct)(void*);
     void (*destruct)(void*);
} Registry;

// Constructor/Destructor
void Registry_construct(void* r);
void Registry_destruct(void* r);

// Registry object methods
int Registry_setRegister(void* r, char reg, int val);
int Registry_performOperation(void* r, char op, signed int* argv, int argc);
int Registry_getRegisterValue(void* r, char reg, signed int* target);
signed int* Registry_translateRegister(void* r, int regNum);

// Utility functions
int Registry_switchRegisters(void* r, signed int* reg1, signed int* reg2);
int Registry_doArithmetic(char op, signed int* r1, signed int* r2);
int Registry_doLogic(char op, signed int* r1, signed int* r2, int destRegIndex);
