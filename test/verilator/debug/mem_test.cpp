#include "../common/Memory.h"

#include <cstdio>

int main()
{
    Memory<> mem{};
    uint32_t value = 0xFFFF;
    uint64_t val2  = 0xAABBCCDD;

    mem.write(10, value);
    value = mem.read<uint32_t>(10);

    printf("%x\n", value);


    return 0;
}