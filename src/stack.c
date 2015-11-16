#include <stdlib.h>
#include <stack.h>

void Stack_push(Stack* s, int val)
{
    *(s->sp) = val;
    s->sp++;
}

void Stack_pop(Stack* s, int* tgt)
{
    s->sp--;
    *tgt = *(s->sp);
    *(s->sp) = 0;
}
void Stack_construct(Stack* s, int max_height)
{
    s->push = Stack_push;
    s->pop = Stack_pop;
    s->s = calloc(sizeof(int), max_height);
    s->sp = s->s;
    s->bp = s->s;
}
void Stack_destroy(Stack* s)
{
    free(s->s);
}
