#ifndef AXI_RESPONDER_H
#define AXI_RESPONDER_H

#include <queue>
#include <cstdio>
#include <map>
#include <cstdint>
#include <vector>
#include <cstdlib>

#ifndef NVDLA_MEM_ADDRESS_WIDTH
    #define NVDLA_MEM_ADDRESS_WIDTH 64
#endif

#define NVDLA_PRIMARY_MEMIF_WIDTH 64

class AXIResponder {
public:
#if NVDLA_PRIMARY_MEMIF_WIDTH == 64
#	define AXI_WDATA_TYPE uint64_t
#	define AXI_WDATA_TYLEN 64
#	define AXI_WDATA_MKPTR &
#	define AXI_WSTRB_TYPE uint8_t
#elif NVDLA_PRIMARY_MEMIF_WIDTH == 512
#	define AXI_WDATA_TYPE uint32_t
#	define AXI_WDATA_TYLEN 32
#	define AXI_WDATA_MKPTR
#	define AXI_WSTRB_TYPE uint64_t
#else
#	error unsupported NVDLA_PRIMARY_MEMIF_WIDTH
#endif

#if NVDLA_MEM_ADDRESS_WIDTH == 32
#	define AXI_ADDR_TYPE uint32_t
#elif NVDLA_MEM_ADDRESS_WIDTH == 64
#	define AXI_ADDR_TYPE uint64_t
#else
#	error unsupported NVDLA_MEM_ADDRESS_WIDTH
#endif

#define AXI_WIDTH NVDLA_PRIMARY_MEMIF_WIDTH

	struct connections
	{
		uint8_t* aw_awvalid;
		uint8_t* aw_awready;
		uint8_t* aw_awid;
		uint8_t* aw_awlen;
		AXI_ADDR_TYPE* aw_awaddr;
		
		uint8_t* w_wvalid;
		uint8_t* w_wready;
		AXI_WDATA_TYPE* w_wdata;
		AXI_WSTRB_TYPE* w_wstrb;
		uint8_t* w_wlast;
		
		uint8_t* b_bvalid;
		uint8_t* b_bready;
		uint8_t* b_bid;
		
		uint8_t* ar_arvalid;
		uint8_t* ar_arready;
		uint8_t* ar_arid;
		uint8_t* ar_arlen;
		AXI_ADDR_TYPE* ar_araddr;
		
		uint8_t* r_rvalid;
		uint8_t* r_rready;
		uint8_t* r_rid;
		uint8_t* r_rlast;
		AXI_WDATA_TYPE* r_rdata;
	};

private:

#define AXI_BLOCK_SIZE 4096

	const static int AXI_R_LATENCY = 32;
	const static int AXI_R_DELAY = 0;

	struct axi_r_txn
    {
		int rvalid;
		int rlast;
		AXI_WDATA_TYPE rdata[AXI_WIDTH / AXI_WDATA_TYLEN];
		uint8_t rid;
	};

	std::queue<axi_r_txn> r_fifo;
	std::queue<axi_r_txn> r0_fifo;
	
	struct axi_aw_txn
    {
		uint8_t awid;
		AXI_ADDR_TYPE awaddr;
		uint8_t awlen;
	};

	std::queue<axi_aw_txn> aw_fifo;

	struct axi_w_txn
    {
		AXI_WDATA_TYPE wdata[AXI_WIDTH / AXI_WDATA_TYLEN];
		AXI_WSTRB_TYPE wstrb;
		uint8_t wlast;
	};
	std::queue<axi_w_txn> w_fifo;
	
	struct axi_b_txn
    {
		uint8_t bid;
	};
	
    std::queue<axi_b_txn> b_fifo;
	
	std::map<uint32_t, std::vector<uint8_t> > ram;
	
	connections dla;
	const char *name;
	
public:	
	AXIResponder(connections _dla, const char *_name) noexcept;

	uint8_t read(uint32_t addr);
	
	void write(uint32_t addr, uint8_t data);
	
	void eval();
};



#endif