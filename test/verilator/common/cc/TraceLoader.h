#ifndef TRACE_LOADER_H
#define TRACE_LOADER_H

#include <cstdint>
#include <queue>
#include <map>
#include <fcntl.h>

#include "AXIResponder.h"
#include "CSBMaster.h"

enum class TraceCommand : uint8_t
{
    WFI = 1,
    WriteRegister = 2,
    ReadRegister = 3,
    DumpMemory = 4,
    LoadMemory = 5,
    RegisterSyncpoint = 6,
    SetInterruptRegisters = 7,
    SyncpointCheckCRC = 8,
    SyncpointCheckNothing = 9,
    Close = 0xFF
};

class TraceLoader
{
	
    enum axi_opc
    {
		AXI_LOADMEM,
		AXI_DUMPMEM
	};

	struct axi_op
    {
		axi_opc opcode;
		uint32_t addr;
		uint32_t len;
		const uint8_t *buf;
		const char *fname;
	};

	std::queue<axi_op> opq;
	
	enum syncpt_opc
    {
		SYNCPT_CRC,
		SYNCPT_NOOP
	};
	
	struct syncpt_op
    {
		syncpt_opc opcode;
		uint32_t base, size, crc;
	};

	std::map<uint32_t, syncpt_op> syncpts;
	
	CSBMaster *csb;
	AXIResponder *axi_dbb;
	
	int _test_passed;

public:
	enum stop_type
    {
		TRACE_CONTINUE = 0,
		TRACE_AXIEVENT,
		TRACE_WFI,
		TRACE_SYNCPT_MASK = 0x80000000
	};

	TraceLoader(CSBMaster *_csb, AXIResponder *_axi_dbb) noexcept;
	
	void load(const char *fname);
	
	void axievent();
	
	void syncpt(uint32_t id);
	
	int test_passed() const noexcept;
};



#endif