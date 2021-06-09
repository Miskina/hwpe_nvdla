#ifndef HWPE_NVDLA_MEMORY_H
#define HWPE_NVDLA_MEMORY_H

#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <string>

#define PAGE_SIZE 4096


class Memory
{

public:

    Memory(std::string&& name, const size_t block_size = PAGE_SIZE) noexcept 
        : name_(std::forward<std::string>(name)), block_size_(block_size)
    {
    }

    template<typename DataType, typename AddressType>
    DataType read(const AddressType address) const noexcept
    {
        static_assert(std::is_integral<AddressType>::value,
                      "AddressType must be an integral type!");
        static_assert(std::is_integral<DataType>::value,
                      "The DataType must be an integral type!");

        auto ram_it = ram_.find(address / block_size_);
        if (ram_it == ram_.end())
        {
            return 0;
        }

        const auto& mem_block = ram_it->second;
        const AddressType misalignment = address % alignof(DataType);
        DataType value = *(reinterpret_cast<const DataType*>(&mem_block[(address % block_size_) - misalignment]));

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
    void write(const AddressType address, DataType data) noexcept
    {
        static_assert(std::is_integral<DataType>::value,
                      "The DataType must be an integral type!");
        static_assert(std::is_integral<AddressType>::value,
                      "AddressType must be an integral type!");

        auto& mem_block = ram_[address / block_size_];
        mem_block.resize(block_size_, 0);

        const AddressType misalignment = address % alignof(DataType);
        const AddressType aligned_address = (address % block_size_) - misalignment;

        DataType* mem_block_data = reinterpret_cast<DataType*>(&mem_block[aligned_address]);

        if (!misalignment)
        {
            *mem_block_data = data;
            return;
        }
        
        // Avoid bug with type inference automating to int for integer literals.
        // Otherwise the mask is 0 for misalignment larger than 3 (which can happen with 64-bit integers). 
        constexpr DataType one = 1;

        const DataType value = data << misalignment * 8;
        const DataType mask = (one << (misalignment * 8)) - one;
        *mem_block_data &= mask;
        *mem_block_data |= value;            

        const int to_aligned = alignof(DataType) - misalignment;
        const AddressType next_addr = address + to_aligned; 
        const DataType next_data = (read<DataType>(next_addr) & ~mask) | (data >> (to_aligned * 8));

        write(next_addr, next_data);
    }


    const std::string& name() const noexcept;

    size_t block_size() const noexcept;

private:

    std::unordered_map<uint32_t, std::vector<uint8_t>> ram_;
    std::string name_;
    const size_t block_size_;

};


#endif // HWPE_NVDLA_MEMORY_H