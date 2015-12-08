typedef struct {
     char* registers;
     char* extregisters;
     int (*setRegister)(void*, char, int);
     int (*switchRegisters)(void*, char*, char*);
     int (*performOperation)(void*, char, char*);
     int (*getRegisterValue)(void*, char, int*);
     void (*construct)(void*);
     void (*destruct)(void*);
} Registry;

void Registry_construct(void* r);
void Registry_destruct(void* r);
int Registry_setRegister(void* r, char reg, int val);
int Registry_switchRegisters(void* r, char* reg1, char* reg2);
int Registry_performOperation(void* r, char op, char* args);
int Registry_getRegisterValue(void* r, char reg, int* target);
