#include <stdlib.h>
#include <registry.h>
#include <bcerrors.h>

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
Registry_switchRegisters(void* r, char* reg1, char* reg2)
{
    Registry* this = (Registry*) r;
    *reg1 ^= *reg2;
    *reg2 ^= *reg1;
    *reg1 ^= *reg2;
    return 0;
}

int
Registry_performOperation(void* r, char op, char* args)
{
    Registry* this = (Registry*) r;
    return 0;
}

void
Registry_construct(void* r)
{
    Registry* this = (Registry*) r;
    this->construct = Registry_construct;
    this->destruct = Registry_destruct;
    this->setRegister = Registry_setRegister;
    this->switchRegisters = Registry_switchRegisters;
    this->registers = calloc(sizeof(char), 5);
    this->extregisters = calloc(sizeof(char), 5);
}

void
Registry_destruct(void* r)
{
    Registry* this = (Registry*) r;
    free(this->registers);
    free(this->extregisters);
}
