#ifndef MEMORY_H
#define MEMORY_H

#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <cassert>

#define PAGE_SIZE 4096

template<typename AddressType = uint32_t,
         size_t BlockSize = PAGE_SIZE>
class Memory
{
    static_assert(std::is_integral<AddressType>::value, "AddressType must be an integral type!");

public:

    template<typename DataType>
    DataType read(AddressType address) const noexcept
    {
        static_assert(std::is_integral<DataType>::value,
                      "The DataType must be an integral type!");

        auto ram_it = ram_.find(address / BlockSize);
        if (ram_it == ram_.end())
        {
            return 0;
        }

        const auto& mem_block = ram_it->second;
        const AddressType misalignment = address % alignof(DataType);
        DataType value = *(reinterpret_cast<const DataType*>(&mem_block[(address % BlockSize) - misalignment]));

        if (!misalignment) 
        {
            return value;
        }

        value >>= misalignment * 8;
        value |= read<DataType>(address + alignof(DataType) - misalignment) << misalignment * 8;

        return value;
    }

    template<typename DataType>
    void write(AddressType address, DataType data) noexcept
    {
        static_assert(std::is_integral<DataType>::value,
                      "The DataType must be an integral type!");
        
        auto& mem_block = ram_[address / BlockSize];
        mem_block.resize(BlockSize, 0);

        const AddressType misalignment = address % alignof(DataType);
        const AddressType aligned_address = (address % BlockSize) - misalignment;

        DataType* mem_block_data = reinterpret_cast<DataType*>(&mem_block[aligned_address]);

        if (!misalignment)
        {
            *mem_block_data = data;
            return;
        }


        DataType value = data << misalignment * 8;
        DataType mask = (1 << (misalignment * 8)) - 1;
        *mem_block_data &= mask;
        *mem_block_data |= value;            

        const AddressType next_addr = aligned_address + alignof(DataType);
        DataType next_data = (read<DataType>(next_addr) & ~mask) | (data >> ((alignof(DataType) - misalignment) * 8));

        write(next_addr, next_data);
    }

private:

    std::unordered_map<AddressType, std::vector<uint8_t>> ram_;

};


#endif // MEMORY_H