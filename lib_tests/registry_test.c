#include <stdio.h>
#include <registry.h>

int main()
{
    Registry r;
    Registry_construct(&r);
    r.setRegister(&r, '1', 30);
    r.setRegister(&r, '2', 45);
    int argv[2];
    int argv2[1];
    argv2[0] = '1';
    argv[0] = '1';
    argv[1] = '2';
    int argc = 2;
    int error;
    error = r.performOperation(&r, '+', argv, argc);
    printf("Add %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, ',', argv, argc);
    printf("Swap %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, '+', argv, argc);
    printf("Add %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, '!', argv2, 1);
    printf("Not %d %d %d\n", r.registers[1], r.registers[2],error);
    printf("I wanna see this now %d\n", r.registers[3]);
    r.destruct(&r);
    return 0;
}

