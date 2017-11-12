#include <stdio.h>
#include <stdlib.h>
#include <stack.h>

void Stack_push(void* s, int val)
{
    Stack* this = (Stack*) s;
    char *new_spp = (char*) *this->spp;
    **(this->spp) = val;
    new_spp += sizeof(int);
    (*this->spp) = new_spp;
}

void Stack_pop(void* s, int* tgt)
{
    Stack* this = (Stack*) s;
    char* new_spp = (char*) *this->spp;
    new_spp -= sizeof(int);
    (*this->spp) = new_spp;
    *tgt = **(this->spp);
}
void Stack_printStack(void* s)
{
    Stack* this = (Stack*) s;
    int* sp = (char*) (*(this->spp)) - sizeof(int);
    int* bp = *(this->bpp);
    while(sp >= bp) {
        printf("   Stack %02d (%x): ", (int)(sp - bp), sp);

        int x = sizeof(int) - 1;
        for( ; x >= 0 ; x-- ) {
            printf("%02x ", (*sp >> (x * 8)) & 0xFF);
        }

        printf("\n");

        sp--;
    }
}
void Stack_construct(void* s, int max_height, int*** spp, int*** bpp)
{
    Stack* this = (Stack*) s;
    this->push = Stack_push;
    this->pop = Stack_pop;
    this->s = calloc(sizeof(int), max_height);
    if(
       (((int)spp) == 0)
       || (((int)bpp) == 0)
      ) {
        this->spp = &(this->sp);
        this->bpp = &(this->bp);
        this->sp = this->s;
        this->bp = this->s;
    } else {
        *spp = this->s;
        *bpp = this->s;
        this->spp = spp;
        this->bpp = bpp;
    }
    this->construct = Stack_construct;
    this->destruct = Stack_destruct;
    this->printStack = Stack_printStack;
}
void Stack_destruct(void* s)
{
    Stack* this = (Stack*) s;
    free(this->s);
}
