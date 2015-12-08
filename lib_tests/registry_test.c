#include <registry.h>

int main()
{
    Registry r;
    Registry_construct(&r);
    r.setRegister(&r, '1', 30);
    printf("%d\n", r.registers[1]);
    r.destruct(&r);
    return 0;
}

