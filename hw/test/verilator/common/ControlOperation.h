#ifndef HWPE_NVDLA_CONTROL_OPERATION_H
#define HWPE_NVDLA_CONTROL_OPERATION_H

#include <cstdint>

struct ControlOperationResponse
{
    bool* valid = nullptr;
    uint32_t *data = nullptr;

    constexpr ControlOperationResponse() = default;
    constexpr ControlOperationResponse(bool* valid_, uint32_t* data_) : valid(valid_), data(data_) { }


    constexpr operator bool() const noexcept
    {
        return valid && data;
    }
};

struct ControlOperation
{
    bool wen = false;
    uint32_t addr = 0;
    uint32_t data = 0;
    uint8_t be = 0;
    uint32_t id = 0;

    constexpr ControlOperation() = default;
    constexpr ControlOperation(bool wen_, uint32_t addr_, uint32_t data_, uint8_t be_, uint32_t id_) :
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

    static constexpr ControlOperation Read(uint32_t addr = 0) noexcept
    {
        return {true, addr, 0, 0xFF, 0};
    }

    static constexpr ControlOperation Write(uint32_t addr = 0, uint32_t data = 0) noexcept
    {
        return {false, addr, data, 0xFF, 0};
    }

};



#endif // HWPE_NVDLA_CONTROL_OPERATION_H