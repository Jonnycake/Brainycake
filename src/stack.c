#include <stdlib.h>
#include <stack.h>

void Stack_push(void* s, int val)
{
    Stack* this = (Stack*) s;
    *(this->sp) = val;
    this->sp++;
}

void Stack_pop(void* s, int* tgt)
{
    Stack* this = (Stack*) s;
    this->sp--;
    *tgt = *(this->sp);
    *(this->sp) = 0;
}
void Stack_construct(void* s, int max_height)
{
    Stack* this = (Stack*) s;
    this->push = Stack_push;
    this->pop = Stack_pop;
    this->s = calloc(sizeof(int), max_height);
    this->sp = this->s;
    this->bp = this->s;
    this->construct = Stack_construct;
    this->destruct = Stack_destruct;
}
void Stack_destruct(void* s)
{
    Stack* this = (Stack*) s;
    free(this->s);
}
