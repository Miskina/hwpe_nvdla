#include "include/TraceLoader.h"


TraceLoader::TraceLoader(CSBMaster *_csb, AXIResponder *_axi_dbb) noexcept
{
    csb = _csb;
    axi_dbb = _axi_dbb;
    _test_passed = 1;
}

void TraceLoader::load(const char *fname)
{
    int fd = open(fname, O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Error while trying to open trace file: %s", fname);
        abort();
    }

    TraceCommand trace_command;
#define VERILY_READ(p, n) do { if (read(fd, (p), (n)) != (n)) { fprintf(stderr, "short read %d\n", __LINE__); abort(); } } while(0)
    do
    {
        VERILY_READ(&trace_command, 1);
        
        switch (trace_command)
        {
        case TraceCommand::WFI:
        {
            printf("CMD: wait\n");
            csb->ext_event(TRACE_WFI);
            break;
        }
        case TraceCommand::WriteRegister:
        {
            uint32_t addr;
            uint32_t data;
        
            VERILY_READ(&addr, 4);
            VERILY_READ(&data, 4);
            printf("CMD: write_reg %08x %08x\n", addr, data);
            csb->write(addr, data);
            break;
        }
        case TraceCommand::ReadRegister:
        {
            uint32_t addr;
            uint32_t mask;
            uint32_t data;
            
            VERILY_READ(&addr, 4);
            VERILY_READ(&mask, 4);
            VERILY_READ(&data, 4);
            printf("CMD: read_reg %08x %08x %08x\n", addr, mask, data);
            csb->read(addr, mask, data);
            break;
        }
        case TraceCommand::DumpMemory:
        {
            uint32_t addr;
            uint32_t len;
            uint8_t *buf;
            uint32_t namelen;
            char *fname;
            axi_op op;
            
            VERILY_READ(&addr, 4);
            VERILY_READ(&len, 4);
            buf = (uint8_t *)malloc(len);
            VERILY_READ(buf, len);
            
            VERILY_READ(&namelen, 4);
            fname = (char *) malloc(namelen+1);
            VERILY_READ(fname, namelen);
            fname[namelen] = 0;
            
            op.opcode = AXI_DUMPMEM;
            op.addr = addr;
            op.len = len;
            op.buf = buf;
            op.fname = fname;
            opq.push(op);
            csb->ext_event(TRACE_AXIEVENT);
            
            printf("CMD: dump_mem %08x bytes from %08x -> %s\n", len, addr, fname);
            break;
        }
        case TraceCommand::LoadMemory:
        {
            uint32_t addr;
            uint32_t len;
            uint8_t *buf;
            axi_op op;
            
            VERILY_READ(&addr, 4);
            VERILY_READ(&len, 4);
            buf = (uint8_t *)malloc(len);
            VERILY_READ(buf, len);
            
            op.opcode = AXI_LOADMEM;
            op.addr = addr;
            op.len = len;
            op.buf = buf;
            opq.push(op);
            csb->ext_event(TRACE_AXIEVENT);
            
            printf("CMD: load_mem %08x bytes to %08x\n", len, addr);
            break;
        }
        case TraceCommand::RegisterSyncpoint:
        {
            uint32_t id, mask;
            
            VERILY_READ(&id, 4);
            VERILY_READ(&mask, 4);
            
            csb->register_syncpt(TRACE_SYNCPT_MASK | id, mask);
            
            printf("CMD: register syncpt %d = %08x\n", id, mask);
            break;
        }
        case TraceCommand::SetInterruptRegisters:
        {
            uint32_t status, mask;
            
            VERILY_READ(&status, 4);
            VERILY_READ(&mask, 4);
            
            csb->set_intr_registers(status, mask);
            
            printf("CMD: set interrupt registers: status %08x, mask %08x\n", status, mask);
            break;
        }
        case TraceCommand::SyncpointCheckCRC:
        {
            uint32_t spid, base, size, crc;
            syncpt_op op;
            
            VERILY_READ(&spid, 4);
            VERILY_READ(&base, 4);
            VERILY_READ(&size, 4);
            VERILY_READ(&crc, 4);
            
            op.opcode = SYNCPT_CRC;
            op.base = base;
            op.size = size;
            op.crc = crc;
            syncpts[spid] = op;
            
            printf("CMD: syncpt action %d = CRC(%08x + %08x) -> %08x\n", spid, base, size, crc);
            
            break;
        }
        case TraceCommand::SyncpointCheckNothing:
        {
            uint32_t spid;
            syncpt_op op;
            
            VERILY_READ(&spid, 4);
            
            op.opcode = SYNCPT_NOOP;
            syncpts[spid] = op;
            
            printf("CMD: syncpt action %d = ignore\n", spid);
            
            break;
        }
        case TraceCommand::Close:
            printf("CMD: done\n");
            break;
        default:
            printf("unknown command %hhu\n", trace_command);
            abort();
        }

    } while (trace_command != TraceCommand::Close);
    
    close(fd);
}

void TraceLoader::axievent()
{
    if (opq.empty())
    {
        printf("extevent with nothing in the queue?\n");
        abort();
    }
    
    axi_op &op = opq.front();
    
    
    if (((op.addr & 0x80000000) != 0x80000000) || !axi_dbb)
    {
        printf("AXI event to bad offset\n");
        abort();
    }
    
    
    switch(op.opcode)
    {
    case AXI_LOADMEM:
    {
        const uint8_t *buf = op.buf;
        
        printf("AXI: loading memory at 0x%08x\n", op.addr);
        while (op.len)
        {
            axi_dbb->write(op.addr, *buf);
            buf++;
            op.addr++;
            op.len--;
        }
        break;
    }
    case AXI_DUMPMEM:
    {
        int fd;
        const uint8_t *buf = op.buf;
        int matched = 1;
        
        printf("AXI: dumping memory to %s\n", op.fname);
        fd = creat(op.fname, 0666);
        if (!fd)
        {
            perror("creat(dumpmem)");
            break;
        }
        while (op.len)
        {
            uint8_t da = axi_dbb->read(op.addr);
            write(fd, &da, 1);
            if (da != *buf && matched)
            {
                printf("AXI: FAIL: mismatch at memory address %08x (exp 0x%02x, got 0x%02x), and maybe others too\n", op.addr, *buf, da);
                matched = 0;
                _test_passed = 0;
            }
            buf++;
            op.addr++;
            op.len--;
        }
        close(fd);
        
        if (matched)
        {
            printf("AXI: memory dump matched reference\n");
        }
        break;
    }
    default:
        abort();
    }
    
    opq.pop();
}


void TraceLoader::syncpt(uint32_t id)
{
    id &= ~TRACE_SYNCPT_MASK;
    
    syncpt_op op = syncpts[id];
    syncpts.erase(id);

    switch (op.opcode)
    {
    case SYNCPT_NOOP:
        break;
    case SYNCPT_CRC:
    {
        uint32_t crc = ~0;
        uint32_t addr = op.base, len = op.size;
        
        
        if (((op.base & 0x80000000) != 0x80000000) || !axi_dbb)
        {
            printf("AXI event to bad offset\n");
            abort();
        }
        
        while (len)
        {
            uint8_t da = axi_dbb->read(addr);
            
            crc ^= da;
            for (int i = 0; i < 8; i++)
            {
                crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
            }
            
            addr++;
            len--;
        }
        
        crc = ~crc;
        
        if (crc != op.crc)
        {
            printf("*** FAIL: CRC mismatch: %08x + %08x should have been %08x, was %08x\n", op.base, op.size, crc, op.crc);
            _test_passed = 0;
        } else
        {
            printf("*** CRC matched %08x + %08x -> %08x\n", op.base, op.size, crc);
        }
        
        break;
    }
    default:
        abort();
    }
}
	
int TraceLoader::test_passed() const noexcept
{
    return _test_passed;
}
