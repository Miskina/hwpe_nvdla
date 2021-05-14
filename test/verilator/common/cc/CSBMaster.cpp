#include "CSBMaster.h"
#include <verilated.h>

CSBMaster::CSBMaster(VNV_nvdla *_dla) noexcept
{
		dla = _dla;
		
		dla->csb2nvdla_valid = 0;
		_test_passed = 1;
		quiet = 0;
}


void CSBMaster::read(uint32_t addr, uint32_t mask, uint32_t data, enum why why) noexcept
{
    csb_op op;

    op.is_ext = 0;
    op.write = 0;
    op.why = why;
    op.addr = addr;
    op.mask = mask;
    op.data = data;
    op.tries = 10;
    op.reading = 0;

    opq.push(op);
}


void CSBMaster::write(uint32_t addr, uint32_t data) noexcept
{
    csb_op op;

    op.is_ext = 0;
    op.write = 1;
    op.why = FROM_TRACE;
    op.addr = addr;
    op.data = data;

    opq.push(op);
}



void CSBMaster::ext_event(int ext) noexcept
{
    csb_op op;
    
    op.is_ext = ext;
    opq.push(op);
}


int CSBMaster::eval(int noop)
{
    if (dla->nvdla2csb_wr_complete)
        if (!quiet) printf("(%lu) write complete from CSB\n", ticks);
    
    dla->csb2nvdla_valid = 0;
    if (opq.empty() && syncpts.empty())
        return 0;
    if (opq.empty() && !syncpts.empty()) {
        /* Perhaps we need to synthesize some mask reads and interrupt reads? */
        if (!quiet) printf("(%lu) CSB switching to interrupt polling\n", ticks);
        quiet = 1;
        read(intr_mask_reg, 0, 0, IS_MASK);
        read(intr_status_reg, 0, 0, IS_STATUS);
    }

    csb_op &op = opq.front();
    
    if (op.is_ext && !noop) {
        int ext = op.is_ext;
        opq.pop();
        
        return ext;
    }
    
    if (!op.write && op.reading && dla->nvdla2csb_valid) {
        if (!quiet) printf("(%lu) read response from nvdla: %08x\n", ticks, dla->nvdla2csb_data);
        
        if (op.why == IS_MASK) {
            last_mask = dla->nvdla2csb_data;
            opq.pop();
        } else if (op.why == IS_STATUS) {
            last_status = dla->nvdla2csb_data;
            opq.pop();
            
            uint32_t status = ~last_mask & last_status;
            for (auto it = syncpts.begin();
                    it != syncpts.end(); it++) {
                if ((status & it->second) != it->second)
                    continue;
                
                printf("(%lu) syncpt %08x\n", ticks, it->first);
                write(intr_status_reg, it->second);
                syncpts.erase(it);
                
                return it->first;
            }
        } else if ((dla->nvdla2csb_data & op.mask) != (op.data & op.mask)) {
            op.reading = 0;
            op.tries--;
            if (!quiet) printf("(%lu) invalid response -- trying again\n", ticks);
            if (!op.tries) {
                if (!quiet) printf("(%lu) ERROR: timed out reading response\n", ticks);
                _test_passed = 0;
                opq.pop();
            }
        } else
            opq.pop();
    }
    
    if (!op.write && op.reading)
        return 0;
    
    if (noop)
        return 0;
    
    if (!dla->csb2nvdla_ready) {
        if (!quiet) printf("(%lu) CSB stalled...\n", ticks);
        return 0;
    }
    
    if (op.write) {
        dla->csb2nvdla_valid = 1;
        dla->csb2nvdla_addr = op.addr;
        dla->csb2nvdla_wdat = op.data;
        dla->csb2nvdla_write = 1;
        dla->csb2nvdla_nposted = 0;
        if (!quiet) printf("(%lu) write to nvdla: addr %08x, data %08x\n", ticks, op.addr, op.data);
        opq.pop();
    } else {
        dla->csb2nvdla_valid = 1;
        dla->csb2nvdla_addr = op.addr;
        dla->csb2nvdla_write = 0;
        if (!quiet) printf("(%lu) read from nvdla: addr %08x\n", ticks, op.addr);
        
        op.reading = 1;
    }
    
    return 0;
}

bool CSBMaster::done() const noexcept
{
    return opq.empty() && syncpts.empty();
}

int CSBMaster::test_passed() const noexcept
{
    return _test_passed;
}


void CSBMaster::set_intr_registers(uint32_t status, uint32_t mask) noexcept
{
    intr_status_reg = status;
    intr_mask_reg = mask;
}

void CSBMaster::register_syncpt(uint32_t syncpt, uint32_t mask) noexcept
{
    syncpts[syncpt] = mask;
}