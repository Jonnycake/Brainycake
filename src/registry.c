#include <stdlib.h>
#include <registry.h>
#include <bcerrors.h>

void
Registry_construct(void* r)
{
    Registry* this = (Registry*) r;
    this->construct = Registry_construct;
    this->destruct = Registry_destruct;
    this->setRegister = Registry_setRegister;
    this->getRegisterValue = Registry_getRegisterValue;
    this->performOperation = Registry_performOperation;
    this->translateRegister = Registry_translateRegister;
    this->switchRegisters = Registry_switchRegisters;

    // 5 General Purpose char Registers
    this->registers = calloc(sizeof(char), CHAR_REG_COUNT);

    // 5 General Purpose int registers with base, 
    // stack, tape, and instruction pointer registers
    this->extregisters = calloc(sizeof(int), EXT_REG_COUNT);
}

void
Registry_destruct(void* r)
{
    Registry* this = (Registry*) r;
    free(this->registers);
    free(this->extregisters);
}

int
Registry_setRegister(void* r, char reg, int val)
{
    Registry* this = (Registry*) r;
    int error = 0;
    switch(reg)
    {
        case '0':
            error = ERROR_RO;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
            this->registers[reg - 48] = (char) val;
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            this->extregisters[reg - 53] = val;
            break;
        default:
            error = ERROR_UNKNOWN;
            break;
    }
    return error;
}

int
Registry_getRegisterValue(void* r, char reg, int* target)
{
    Registry* this = (Registry*) r;
    char* charTgt = (char*)target;
    int error = 0;
    switch(reg)
    {
        case '0':
            break;
        case '1':
        case '2':
        case '3':
        case '4':
            *charTgt = this->registers[reg - 48];
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            *target = this->extregisters[reg - 53];
            break;
        default:
            error = ERROR_UNKNOWN;
            break;
    }
    return error;
}

int
Registry_performOperation(void* r, char op, int* argv, int argc)
{
    int error = ERROR_NORMAL;
    Registry* this = (Registry*) r;
    switch(op)
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            {
                int* reg1;
                int* reg2;
                if(argc != 0 && argc != 2) {
                    error = ERROR_SYNTAX;
                }
                // If no arguments, reg1 is $1, reg2 is pointer on tape
                else if(argc == 0) {
                    reg1 = this->translateRegister(r, '1');
                    reg2 = this->translateRegister(r, 't');
                }
                else {
                    reg1 = this->translateRegister(r, argv[0]);
                    reg2 = this->translateRegister(r, argv[1]);
                    if(reg1 < 0 || reg2 < 0) {
                        error = ERROR_UNKNOWN;
                    }
                }
                if(!error) {
                    Registry_doArithmetic(op, reg1, reg2);
                }
            }
            break;
        case '&':
        case '|':
        case '^':
            {
                int* reg1;
                int* reg2;
                if(argc != 0 && argc != 2) {
                    error = ERROR_SYNTAX;
                }
                // If no arguments, reg1 is $1, reg2 is pointer on tape
                else if(argc == 0) {
                    reg1 = this->translateRegister(r, '1');
                    reg2 = this->translateRegister(r, 't');
                }
                else {
                    reg1 = this->translateRegister(r, argv[0]);
                    reg2 = this->translateRegister(r, argv[1]);
                    if(reg1 < 0 || reg2 < 0) {
                        error = ERROR_UNKNOWN;
                    }
                }
                if(!error) {
                    error = Registry_doLogic(op, reg1, reg2);
                }
            }
            break;

        case ',':
            {
                int* reg1;
                int* reg2;
                if(argc != 0 && argc != 2) {
                    error = ERROR_SYNTAX;
                }
                // If no arguments, reg1 is $1, reg2 is pointer on tape
                else if(argc == 0) {
                    reg1 = this->translateRegister(r, '1');
                    reg2 = this->translateRegister(r, 't');
                }
                else {
                    reg1 = this->translateRegister(r, argv[0]);
                    reg2 = this->translateRegister(r, argv[1]);
                    if(reg1 < 0 || reg2 < 0) {
                        error = ERROR_UNKNOWN;
                    }
                }
                if(!error) {
                    error = this->switchRegisters(r, reg1, reg2);
                }
            }
            break;
        case '!':
            {
                int* reg1;
                int* reg2 = (int*) 0;
                if(argc != 1 && argc != 0) {
                    error = ERROR_SYNTAX;
                }
                else if(argc == 0) {
                    reg1 = this->translateRegister(r, '1');
                }
                else {
                    reg1 = this->translateRegister(r, argv[0]);
                }
                if(!error) {
                    error = Registry_doLogic(op, reg1, reg2);
                }
            }
            break;
        default:
            error = ERROR_BADOP;
            break;
    }
    return error;
}

int*
Registry_translateRegister(void* r, int regNum)
{
    Registry* this = (Registry*) r;
    int* reg = (int*)-1;
    switch(regNum)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
            reg = (int*) &(this->registers[regNum - 48]);
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            reg = (int*) &(this->extregisters[regNum - 53]);
            break;
        case 't':
            reg = (int*) &(this->extregisters[TAPE_PTR]);
            break;
        case 'i':
            reg = (int*) &(this->extregisters[INSTRUCTION_PTR]);
            break;
        case 's':
            reg = (int*) &(this->extregisters[STACK_PTR]);
            break;
        case 'b':
            reg = (int*) &(this->extregisters[BASE_PTR]);
            break;
    }
    return reg;
}

int
Registry_switchRegisters(void* r, int* reg1, int* reg2)
{
    return ERROR_NORMAL;
}
int
Registry_doArithmetic(char op, int* r1, int* r2)
{
    switch(op)
    {
        case '+':
            *r1 += *r2;
            break;
        case '-':
            *r1 -= *r2;
            break;
        case '*':
            *r1 *= *r2;
            break;
        case '/':
            *r1 /= *r2;
            break;
        case '%':
            *r1 %= *r2;
            break;
    }
    return ERROR_NORMAL;
}

int
Registry_doLogic(char op, int* r1, int* r2)
{
    switch(op)
    {
        case '&':
            *r1 &= (int)*r2;
            break;
        case '|':
            *r1 |= (int)*r2;
            break;
        case '^':
            *r1 ^= (int)*r2;
            break;
        case '!':
            *r1 = (int) ~*r1;
            break;
    }
    return ERROR_NORMAL;
}
