#ifndef HWPE_NVDLA_FABRIC_CONTROLLER_H
#define HWPE_NVDLA_FABRIC_CONTROLLER_H

#include <string>
#include <cstdio>
#include <vector>

#include "MemoryController.h"
#include "ControlInterface.h"
#include "Interruptable.h"
#include "Util.h"


#define TRACE_COMMAND_NAME(trace_cmd)     \
    _detail::trace_command_names[trace_cmd == TraceCommand::Close ? 10 : static_cast<int>(trace_cmd)]

enum class TraceCommand : uint8_t
{
    Invalid = 0,
    WFI = 1,
    WriteRegister = 2,
    ReadRegister = 3,
    DumpMemory = 4,
    LoadIntoMemory = 5,
    RegisterSyncpoint = 6,
    SetInterruptRegisters = 7,
    SyncpointCheckCRC = 8,
    SyncpointCheckNothing = 9,
    Close = 0xFF
};

namespace _detail
{
    #define TC_I(tc) static_cast<int>(tc)
    static constexpr const char* trace_command_names[11]
    {

        [TC_I(TraceCommand::Invalid)] = "Invalid",
        [TC_I(TraceCommand::WFI)] = "WFI",
        [TC_I(TraceCommand::WriteRegister)] = "WriteRegister",
        [TC_I(TraceCommand::ReadRegister)] = "ReadRegister",
        [TC_I(TraceCommand::DumpMemory)] = "DumpMemory",
        [TC_I(TraceCommand::LoadIntoMemory)] = "LoadIntoMemory",
        [TC_I(TraceCommand::RegisterSyncpoint)] = "RegisterSyncpoint",
        [TC_I(TraceCommand::SetInterruptRegisters)] = "SetInterruptRegisters",
        [TC_I(TraceCommand::SyncpointCheckCRC)] = "SyncpointCheckCRC",
        [TC_I(TraceCommand::SyncpointCheckNothing)] = "SyncpointCheckNothing",
        [10] = "Close"
    };

    #undef TC_I
}


class FabricController : public Interruptable
{

public:

    FabricController(std::string&& name) noexcept;
    
    ~FabricController() noexcept;

    void attach(MemoryController* controller, bool delete_old = true) noexcept;

    void attach(ControlInterface* interface, bool delete_old = true) noexcept;

    void load_trace(const std::string& trace_path);

    bool eval();

    bool test_passed() const noexcept;

private:

    TraceCommand read_trace_command();

    void read_from_trace(uint8_t* data, size_t size);

    template<typename T, typename ... Ts>
    inline void read_from_trace(T* data, Ts*... datas)
    {
        read_from_trace(reinterpret_cast<uint8_t*>(data), sizeof(T));
        (read_from_trace(reinterpret_cast<uint8_t*>(datas), sizeof(Ts)), ...);
    }

    void execute_current_command();
    
    bool trace_file_processed() const noexcept
    {
        return trace_file_ == nullptr; 
    }

    struct SyncPoint 
    {
        constexpr SyncPoint(uint32_t id_, uint32_t mask_ = 0) noexcept : id(id_), mask(mask_) {}

        uint32_t id = 0;
        uint32_t mask = 0;
        uint32_t crc = 0;
        uint32_t base = 0;
        uint32_t size = 0;
        bool processed = false;

        bool is_noop() const noexcept
        {
            return !(crc || base || size);
        }
    };

    SyncPoint& find_syncpoint(const uint32_t sp_id) noexcept;

    std::string name_;

    MemoryController* memory_ctrl_ = nullptr;
    ControlInterface* ctrl_intf_ = nullptr;

    FILE* trace_file_ = nullptr;

    TraceCommand current_command = TraceCommand::Invalid;


    std::vector<SyncPoint> sync_points_{};
    int sync_points_to_process = 0;

    bool sync_points_finished() const noexcept;
    void process_sync_points() noexcept;
    void process_sync_point(SyncPoint& sync_point) noexcept;

    uint32_t interrupt_status_addr = 0;
    bool     interrupt_status_valid = false;
    uint32_t interrupt_status = 0;
    uint32_t interrupt_mask_addr = 0;
    bool     interrupt_mask_valid = false;
    uint32_t interrupt_mask = 0;

    bool test_passed_ = true;
};


#endif // HWPE_FABRIC_CONTROLLER_H