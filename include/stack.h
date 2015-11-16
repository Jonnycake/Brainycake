typedef struct {
    int* s;
    int* sp;
    int* bp;
    void (*push)();
    void (*pop)();
} Stack;

void Stack_push(Stack* s, int val);
void Stack_pop(Stack* s, int* tgt);
void Stack_construct(Stack* s, int max_height);
void Stack_destroy(Stack* s);
