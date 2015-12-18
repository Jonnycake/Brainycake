#include <stdio.h>
#include <registry.h>

int main()
{
    char t[255];
    t[0] = 'a';
    Registry r;
    Registry_construct(&r, (int*)0, (int*) 0, (int*)0, (int*) 0);
    r.setRegister(&r, '1', 200);
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
    printf("Copy %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, '+', argv, argc);
    printf("Add %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, '!', argv2, 1);
    printf("Not %d %d %d\n", r.registers[1], r.registers[2],error);
    error = r.performOperation(&r, 's', argv, 2);
    printf("Swap %d %d %d\n", r.registers[1], r.registers[2], error);
    printf("I wanna see this now %d\n", r.registers[3]);
    argv2[0] = '5';
    argv[0] = '5';
    argv[1] = '6';
    r.setRegister(&r, '6', -100);
    r.setRegister(&r, '5', 300);
    printf("Set register: %d %d\n", r.extregisters[0], r.extregisters[1]);
    error = r.performOperation(&r, '+', argv, argc);
    printf("Add %d %d %d\n", r.extregisters[0], r.extregisters[1],error);
    error = r.performOperation(&r, ',', argv, argc);
    printf("Copy %d %d %d\n", r.extregisters[0], r.extregisters[1],error);
    error = r.performOperation(&r, '+', argv, argc);
    printf("Add %d %d %d\n", r.extregisters[0], r.extregisters[1],error);
    error = r.performOperation(&r, '!', argv2, 1);
    printf("Not %d %d %d\n", r.extregisters[0], r.extregisters[1], error);
    error = r.performOperation(&r, '!', argv2, 1);
    printf("Not %d %d %d\n", r.extregisters[0], r.extregisters[1], error);
    error = r.performOperation(&r, 's', argv, 2);
    printf("Swap %d %d %d\n", r.extregisters[0], r.extregisters[1], error);
    r.printRegisters(&r);
    r.destruct(&r);
    return 0;
}
