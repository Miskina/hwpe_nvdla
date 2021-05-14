#ifndef CSB_MASTER_H
#define CSB_MASTER_H

#include "VNV_nvdla.h"

#include <queue>
#include <map>
#include <cstdio>

extern uint64_t ticks;

class CSBMaster
{
	enum why
	{
		FROM_TRACE,
		IS_MASK,
		IS_STATUS
	};

	struct csb_op
	{
		int is_ext;
		int write;
		int tries;
		int reading;
		enum why why;
		uint32_t addr;
		uint32_t mask;
		uint32_t data;
	};
	
	std::queue<csb_op> opq;
	
	VNV_nvdla *dla;
	
	int _test_passed;
	int quiet;
	
	/* It's kind of gross that we have to have this here -- an otherwise
	 * pure class that doesn't know much about NVDLA internals -- but oh
	 * well. */
	uint32_t intr_status_reg;
	uint32_t intr_mask_reg;
	
	uint32_t last_mask, last_status;
	
	/* Maps from outstanding syncpoint IDs to interrupt masks. */
	std::map<uint32_t, uint32_t> syncpts;
	
public:
	CSBMaster(VNV_nvdla *_dla) noexcept;

	void read(uint32_t addr, uint32_t mask, uint32_t data, enum why why = FROM_TRACE) noexcept;
    
	void write(uint32_t addr, uint32_t data) noexcept;
	
	void ext_event(int ext) noexcept;
	
	int eval(int noop);
	
	bool done() const noexcept;
	
	int test_passed() const noexcept;
	
	void set_intr_registers(uint32_t status, uint32_t mask) noexcept;
	
	void register_syncpt(uint32_t syncpt, uint32_t mask) noexcept;
};

#endif