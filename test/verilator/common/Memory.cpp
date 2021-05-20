#include "Memory.h"

const std::string& Memory::name() const noexcept
{
    return name_;
}

size_t Memory::block_size() const noexcept
{
    return block_size_;
}

