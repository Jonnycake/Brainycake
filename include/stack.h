typedef struct {
    /**************
     * Properties *
     **************/
    int* s;      // Holds the stack
    int** sp;    // Stack pointer
    int** bp;    // Base pointer
    int*** spp;  // Pointer to stack pointer
    int*** bpp;  // Pointer to base pointer

    /***********
     * Methods *
     ***********/
    void (*push)(void*, int);
    void (*pop)(void*, int*);
    void (*construct)(void*, int);
    void (*destruct)(void*);
    void (*printStack)(void*);
} Stack;

void Stack_push(void* s, int val);
void Stack_pop(void* s, int* tgt);
void Stack_construct(void* s, int max_height, int*** spp, int*** bpp);
void Stack_destruct(void* s);
void Stack_printStack(void* s);
