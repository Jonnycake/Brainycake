#include <stdio.h>
#include <stdlib.h>
#include <registry.h>
#include <bcerrors.h>
#include <bcdebug.h>

void
Registry_construct(void* r, signed int* bp, signed int* sp, signed int* ip, signed int* tp)
{
    Registry* this = (Registry*) r;
    this->construct = Registry_construct;
    this->destruct = Registry_destruct;
    this->setRegister = Registry_setRegister;
    this->getRegisterValue = Registry_getRegisterValue;
    this->performOperation = Registry_performOperation;
    this->translateRegister = Registry_translateRegister;
    this->switchRegisters = Registry_switchRegisters;
    this->printRegisters = Registry_printRegisters;
    this->doArithmetic = Registry_doArithmetic;
    this->doLogic = Registry_doLogic;
    this->checkExt = Registry_checkExt;
    this->getEffectiveAddress = Registry_getEffectiveAddress;

    // 5 General Purpose char Registers
    this->registers = calloc(sizeof(char), CHAR_REG_COUNT + (EXT_REG_COUNT * sizeof(long int)));
    this->extregisters = (signed long int*)&(this->registers[CHAR_REG_COUNT]);
    this->extregisters[BASE_PTR] = (signed long int) bp;
    this->extregisters[STACK_PTR] = (signed long int) sp;
    this->extregisters[INSTRUCTION_PTR] = (signed long int) ip;
    this->extregisters[TAPE_PTR] = (signed long int) tp;
    this->extregisters[POINTER_PTR] = (signed long int) &this->extregisters[POINTER_PTR];
}

void
Registry_destruct(void* r)
{
    Registry* this = (Registry*) r;
    free(this->registers);
}

int
Registry_setRegister(void* r, char reg, unsigned long int val)
{
#ifdef TEST
    write_log("Setting register %c to %lx...\n", reg, val);
#endif
    Registry* this = (Registry*) r;
    int error = ERROR_NORMAL;
    signed long int* reg_ptr = this->translateRegister(r, reg);
    char* chardest = (char*) reg_ptr;
    if(this->checkExt(r, reg_ptr)) {
        *reg_ptr = (signed long int) val;
    } else {
        *chardest = (char) val;
    }
#ifdef TEST
    write_log("Returning error value: %d\n", error);
#endif
    return error;
}

int
Registry_getRegisterValue(void* r, char reg, signed int* target)
{
    Registry* this = (Registry*) r;
    char* charTgt = (char*)target;
    int error = ERROR_NORMAL;
    signed int* src = this->translateRegister(r, reg);
    if(this->checkExt(r, src) == 0) {
        *charTgt = (char) *src;
    } else {
        *target = (signed int) *src;
    }
    return error;
}

int
Registry_performOperation(void* r, char op, signed int* argv, int argc)
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
                signed int* reg1;
                signed int* reg2;
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
                    if(reg1 == (signed int*) 0 || reg2 == (signed int*) 0) {
                        error = ERROR_UNKNOWN;
                    }
                }
                if(!error) {
                    this->doArithmetic(r, op, reg1, reg2);
                }
            }
            break;
        case '&':
        case '|':
        case '^':
            {
                signed int* reg1;
                signed int* reg2;
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
                    if(reg1 == (signed int*) 0 || reg2 == (signed int*) 0) {
                        error = ERROR_UNKNOWN;
                    }
                }
                if(!error) {
                    error = Registry_doLogic(r, op, reg1, reg2);
                }
            }
            break;

        case ',':
            {
                char reg1;
                signed int* reg2;
                if(argc != 0 && argc != 2) {
                    error = ERROR_SYNTAX;
                }
                else if(argc == 0) {
                    reg1 = '1';
                    reg2 = this->translateRegister(r, 't');
                }
                else {
                    reg1 = argv[0];
                    reg2 = this->translateRegister(r, argv[1]);
                    printf("Memory addreses: %lx - %lx\n", (unsigned long int) reg1, (unsigned long int) reg2);
                }
               if(error == ERROR_NORMAL) {
                    this->setRegister(r, reg1, *reg2);
               }
            }
            break;
        case '@':
            {
                char reg1;
                signed long int* reg2;
                if(argc != 2) {
                    error = ERROR_SYNTAX;
                }
                else {
                    reg1 = argv[0];
                    reg2 = this->getEffectiveAddress(r, argv[1]);
                }
               if(error == ERROR_NORMAL) {
#ifdef TEST
                    write_log("Calling setRegister with %c, %lx (%lx)...\n", reg1, reg2, *reg2);
#endif
                    this->setRegister(r, reg1, *reg2);
               }
            }
            break;
        case 's':
            {
                signed int* reg1;
                signed int* reg2;
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
                    if(reg1 == (signed int*) 0 || reg2 == (signed int*) 0) {
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
                signed int* reg1;
                signed int* reg2 = (signed int*) 0;
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
                    error = Registry_doLogic(r, op, reg1, reg2);
                }
            }
            break;
        default:
            error = ERROR_BADOP;
            break;
    }
    return error;
}

signed int*
Registry_translateRegister(void* r, char regNum)
{
    Registry* this = (Registry*) r;
    signed int* reg = (signed int*)-1;
    switch(regNum)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
            reg = (signed int*) &(this->registers[regNum - 48]);
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            reg = (signed int*) &(this->extregisters[regNum - 53]);
            break;
        case 't':
            reg = (signed int*) this->extregisters[TAPE_PTR];
            break;
        case 'i':
            reg = (signed int*) this->extregisters[INSTRUCTION_PTR];
            break;
        case 's':
            reg = (signed int*) this->extregisters[STACK_PTR];
            break;
        case 'b':
            reg = (signed int*) this->extregisters[BASE_PTR];
            break;
        case '*':
            reg = (signed int*) this->extregisters[POINTER_PTR];
            break;
        case '&':
            reg = (signed int*) &(this->extregisters[POINTER_PTR]);
            break;
    }
    return reg;
}

signed int*
Registry_getEffectiveAddress(void* r, char regNum)
{
#ifdef TEST
    write_log("Getting the effective address of %c...\n", regNum);
#endif
    Registry* this = (Registry*) r;

    signed long int* reg = (signed int*) 0;
    switch(regNum)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
            reg = (signed long int*) &(this->registers[regNum - 48]);
            break;
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            reg = (signed long int*) &(this->extregisters[regNum - 53]);
            break;
        case 't':
            reg = (signed long int*) &(this->extregisters[TAPE_PTR]);
            break;
        case 'i':
            reg = (signed long int*) &(this->extregisters[INSTRUCTION_PTR]);
            break;
        case 's':
            reg = (signed long int*) &(this->extregisters[STACK_PTR]);
            break;
        case 'b':
            reg = (signed long int*) &(this->extregisters[BASE_PTR]);
            break;
    }
#ifdef TEST
    write_log("The effective address is: %lx\n", reg);
    if(reg) {
        write_log("The value is: %lx\n", *(reg));
    }
#endif
    return reg;
}

int
Registry_switchRegisters(void* r, signed int* reg1, signed int* reg2)
{
    Registry* this = (Registry*) r;
    unsigned char* chardest1;
    unsigned char* chardest2;
    int tmp, tmp2;
    tmp = (int) *reg1;
    tmp2 = (int) *reg2;
    if(this->checkExt(r, reg1) == 0) {
        chardest1 = (unsigned char*) reg1;
        *chardest1 = (unsigned char) tmp2;
    } else {
        *reg1 = tmp2;
    }

    if(this->checkExt(r, reg2) == 0) {
        chardest2 = (unsigned char*) reg2;
        *chardest2 = (unsigned char) tmp;
    } else {
        *reg2 = tmp;
    }
    return ERROR_NORMAL;
}
int
Registry_doArithmetic(void* r, char op, signed int* r1, signed int* r2)
{
    Registry* this = (Registry*) r;
    char isExtended = this->checkExt(r, r1);
    char* charTgt = (char*) r1;
    switch(op)
    {
        case '+':
            if(isExtended) {
                *r1 += *r2;
            } else {
                *charTgt += (char) *r2;
            }
            break;
        case '-':
            if(isExtended) {
                *r1 -= *r2;
            } else {
                *charTgt -= (char) *r2;
            }
            break;
        case '*':
            if(isExtended) {
                *r1 *= *r2;
            } else {
                *charTgt *= (char) *r2;
            }
            break;
        case '/':
            if(isExtended) {
                *r1 /= *r2;
            } else {
                *charTgt /= (char) *r2;
            }
            break;
        case '%':
            if(isExtended) {
                *r1 %= *r2;
            } else {
                *charTgt %= (char) *r2;
            }
            break;
    }
    return ERROR_NORMAL;
}

int
Registry_doLogic(void* r, char op, signed int* r1, signed int* r2)
{
    Registry* this = (Registry*) r;
    int isExtended = this->checkExt(r, r1);
    unsigned char* chardest = (unsigned char*) r1;
    switch(op)
    {
        case '&':
            if(isExtended) {
                *r1 &= (int)*r2;
            } else {
                *chardest &= (char)*r2;
            }
            break;
        case '|':
            if(isExtended) {
                *r1 |= (int)*r2;
            } else {
                *chardest |= (char) *r2;
            }
            break;
        case '^':
            if(isExtended) {
                *r1 ^= (int)*r2;
            } else {
                *chardest ^= (char) *r2;
            }
            break;
        case '!':
            if(isExtended) {
                *r1 = (int) ~*r1;
            } else {
                *chardest = (char) ~*r1;
            }
            break;
        default:
            return ERROR_BADOP;
    }
    return ERROR_NORMAL;
}

void
Registry_printRegisters(void* r)
{
    Registry* this = (Registry*) r;
    int i;
    for(i = 0; i < CHAR_REG_COUNT; i++) {
        printf("Register %02d: %02x\n", i, this->registers[i]);
    }

    for(i = 0; i < EXT_REG_COUNT; i++) {
        int x = sizeof(long int) - 1;
        printf("Register %02d: ", CHAR_REG_COUNT + i);
        for( ; x >= 0 ; x-- ) {
            printf("%02lx ", (this->extregisters[i] >> (x * 8)) & 0xFF);
        }

        printf("\n");
    }
}

char
Registry_checkExt(void* r, signed int* reg)
{
    Registry* this = (Registry*) r;
    char extended = 0;
    if((unsigned char*) reg - this->registers >= CHAR_REG_COUNT) {
        extended = 1;
    }
    return extended;
}
