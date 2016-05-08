#include <stdio.h>
#include <stack.h>

int main()
{
    Stack s;
    int x;
    int** sp;
    int** bp;
    Stack_construct(&s, 1024, &sp, &bp); 
    printf("Pushing 20...\n");
    s.push(&s, 20);
    s.printStack(&s);
    printf("Pushing 50...\n");
    s.push(&s, 50);
    s.printStack(&s);
    printf("Pushing 100...\n");
    s.push(&s, 100);
    s.printStack(&s);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    s.printStack(&s);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    s.printStack(&s);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    s.printStack(&s);
    s.destruct(&s);
    return 0;
}
