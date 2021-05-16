#ifndef HWPE_NVDLA_FABRIC_CONTROLLER_H
#define HWPE_NVDLA_FABRIC_CONTROLLER_H

#include <string>
#include <cstdio>
#include <map>

#include "MemoryController.h"
#include "ControlInterface.h"
#include "Interruptable.h"
#include "Util.h"

#define TRACE_SYNCPT_MASK 0x80000000

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


class FabricController : public Interruptable
{

public:

    FabricController(std::string&& name) noexcept;
    
    ~FabricController() noexcept;

    void attach(MemoryController* controller, bool delete_old = true) noexcept;

    void attach(ControlInterface* interface, bool delete_old = true) noexcept;

    void load_trace(const std::string& trace_path);

    bool eval();


private:

    TraceCommand read_trace_command();

    void read_from_trace(uint8_t* data, size_t size);

    template<typename T, typename ... Ts>
    inline void read_from_trace(T* data, Ts*... datas)
    {
        read_from_trace(static_cast<uint8_t*>(data), sizeof(T));
        (read_from_trace(static_cast<uint8_t*>(datas), sizeof(Ts)), ...);
    }

    bool execute_current_command();

    std::string name_;

    MemoryController* memory_ctrl_ = nullptr;
    ControlInterface* ctrl_intf_ = nullptr;

    FILE* trace_file_ = nullptr;
    bool trace_file_processing_finished_ = true;

    TraceCommand current_command = TraceCommand::Invalid;

    struct SyncPoint 
    {
        constexpr SyncPoint() = default;

        constexpr SyncPoint(uint32_t mask_) : mask(mask_) {}

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


    std::map<uint32_t, SyncPoint> sync_points_;
    int sync_points_to_process = 0;

    bool sync_points_finished() const noexcept;
    void process_sync_points() noexcept;
    void process_sync_point(SyncPoint& sync_point) noexcept;

    uint32_t interrupt_status_addr;
    bool     interrupt_status_valid;
    uint32_t interrupt_status;
    uint32_t interrupt_mask_addr;
    bool     interrupt_mask_valid;
    uint32_t interrupt_mask;
};


#endif // HWPE_FABRIC_CONTROLLER_H