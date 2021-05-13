#ifndef PERIPH_OP_H
#define PERIPH_OP_H

#include <cstdint>

struct PeriphOp
{
    bool wen = false;
    uint32_t addr = 0;
    uint32_t data = 0;
    uint8_t be = 0;
    uint32_t id = 0;

    PeriphOp() = default;
    PeriphOp(bool wen_, uint32_t addr_, uint32_t data_, uint8_t be_, uint32_t id_) :
        wen(wen_), addr(addr_), data(data_), be(be_), id(id_)
    { }

    bool is_write() const noexcept
    {
        return !wen;
    }

    bool is_read() const noexcept
    {
        return wen;
    }

    bool is_noop() const noexcept
    {
        return !addr && !data && !be;
    }


    operator bool() const noexcept
    {
        return is_noop();
    }
};



#endif // PERIPH_OP_H