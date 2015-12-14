#include <stdio.h>
#include <stack.h>

int main()
{
    int x;
    Stack s;
    Stack_construct(&s, 1024);
    printf("Pushing 20...\n");
    s.push(&s, 20);
    printf("Pushing 50...\n");
    s.push(&s, 50);
    printf("Pushing 100...\n");
    s.push(&s, 100);
    s.printStack(&s);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    printf("Pop...");
    s.pop(&s, &x);
    printf("%d\n", x);
    s.printStack(&s);
    s.destruct(&s);
    return 0;
}
