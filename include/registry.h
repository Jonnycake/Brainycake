#define CHAR_REG_COUNT  5
#define EXT_REG_COUNT   10
#define BASE_PTR        EXT_REG_COUNT - 5
#define STACK_PTR       EXT_REG_COUNT - 4
#define INSTRUCTION_PTR EXT_REG_COUNT - 3
#define TAPE_PTR        EXT_REG_COUNT - 2
#define POINTER_PTR     EXT_REG_COUNT - 1

typedef struct {
     // Constructor and destructor
     void (*construct)(void*, signed int*, signed int*, signed int*, signed int*);
     void (*destruct)(void*);

     // Properties
     unsigned char* registers;
     signed long int* extregisters;

     // Methods
     int (*setRegister)(void*, char, int);
     int (*performOperation)(void*, char, signed int*, int);
     int (*getRegisterValue)(void*, char, signed int*);
     signed int* (*translateRegister)(void*, char);
     int (*switchRegisters)(void*, signed int* reg1, signed int* reg2);
     void (*printRegisters)(void*);
     int (*doArithmetic)(void*, char, signed int*, signed int*);
     int (*doLogic)(void*, char, signed int*, signed int*);
     signed int* (*getEffectiveAddress)(void*, char);
     char (*checkExt)(void*, signed int*);
} Registry;

// Constructor/Destructor
void Registry_construct(void* r, signed int* bp, signed int* sp, signed int* ip, signed int* tp);
void Registry_destruct(void* r);

// Registry object methods
int Registry_setRegister(void* r, char reg, int val);
int Registry_performOperation(void* r, char op, signed int* argv, int argc);
int Registry_getRegisterValue(void* r, char reg, signed int* target);
signed int* Registry_translateRegister(void* r, char regNum);
void Registry_printRegisters(void* r);
int Registry_switchRegisters(void* r, signed int* reg1, signed int* reg2);
int Registry_doArithmetic(void* r, char op, signed int* r1, signed int* r2);
int Registry_doLogic(void* r, char op, signed int* r1, signed int* r2);
char Registry_checkExt(void* r, signed int* reg);
signed int* Registry_getEffectiveAddress(void* r, char regNum);
