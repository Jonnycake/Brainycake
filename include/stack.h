typedef struct {
    int* s;
    int* sp;
    int* bp;
    void (*push)(void*, int);
    void (*pop)(void*, int*);
    void (*construct)(void*, int);
    void (*destruct)(void*);
    void (*printStack)(void*);
} Stack;

void Stack_push(void* s, int val);
void Stack_pop(void* s, int* tgt);
void Stack_construct(void* s, int max_height);
void Stack_destruct(void* s);
void Stack_printStack(void* s);
