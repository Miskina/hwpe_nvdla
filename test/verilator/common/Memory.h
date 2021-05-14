#ifndef HWPE_NVDLA_MEMORY_H
#define HWPE_NVDLA_MEMORY_H

#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <string>

#define PAGE_SIZE 4096

template<size_t BlockSize = PAGE_SIZE>
class Memory
{
    static_assert(BlockSize % alignof(int) == 0, "The block size must be at least aligned to int");

public:

    Memory(std::string&& name) noexcept 
        : name_(std::forward<std::string>(name))
    {
    }

    template<typename DataType, typename AddressType>
    DataType read(AddressType address) const noexcept
    {
        static_assert(std::is_integral<AddressType>::value,
                      "AddressType must be an integral type!");
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

        const int to_aligned = alignof(DataType) - misalignment;
        value >>= misalignment * 8;
        value |= read<DataType>(address + to_aligned) << to_aligned * 8;

        return value;
    }


    template<typename AddressType, typename DataType>
    void write(AddressType address, DataType data) noexcept
    {
        static_assert(std::is_integral<DataType>::value,
                      "The DataType must be an integral type!");
        static_assert(std::is_integral<AddressType>::value,
                      "AddressType must be an integral type!");

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
        
        // Avoid bug with type inference automating to int for integer literals.
        // Otherwise the mask is 0 for misalignment larger than 3 (which can happen with 64-bit integers) 
        constexpr DataType one = 1;

        DataType value = data << misalignment * 8;
        DataType mask = (one << (misalignment * 8)) - one;
        *mem_block_data &= mask;
        *mem_block_data |= value;            

        const int to_aligned = alignof(DataType) - misalignment;
        const AddressType next_addr = address + to_aligned; 
        DataType next_data = (read<DataType>(next_addr) & ~mask) | (data >> (to_aligned * 8));

        write(next_addr, next_data);
    }


private:

    size_t page_size_;
    std::unordered_map<uint32_t, std::vector<uint8_t>> ram_;
    std::string name_;

};


#endif // HWPE_NVDLA_MEMORY_H