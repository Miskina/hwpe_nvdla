#include "../common/Memory.h"

#include <cstdio>
#include <cassert>
#include <type_traits>

template <size_t BlockSize, typename DataGenerator, typename AddressGenerator>
bool test(const char* test_name,
          int iterations,
          Memory<BlockSize>& mem,
          DataGenerator data_gen,
          AddressGenerator address_gen)
{
    using DataType = std::invoke_result_t<DataGenerator>;

    for (int i = 0; i < iterations; ++i)
    {
        auto address  = address_gen();
        DataType data = data_gen();
        mem.write(address, data);
        DataType r_data = mem.template read<DataType>(address);
        printf("(%llu) %llx == (%llu) %llx\n", r_data, r_data, data, data);
        if (r_data != data)
        {
            
            printf("[%s] - Failed!\n", test_name);
            printf("[%s] - Read and written values must be equal\n", test_name);
            printf("[%s] - [(%d) %08x] %x == %x\n", test_name, address, address, r_data, data);
            return false;
        }
    }

    printf("[%s] - Success!\n", test_name);
    return true;
}

int main()
{
    Memory<> mem{"Memorija"};

    test("Test_uint32_t",
         1000,
         mem,
         []() -> uint32_t { return 0xEEFFu; },
         [i=0]() mutable { return 4000 + i++; });

    test("Test_uint64_t",
         1000,
         mem,
         []() -> uint64_t { return 0xDEADBEEFul; },
         [i=0]() mutable { return 4000 + i++; });

    return 0;
}