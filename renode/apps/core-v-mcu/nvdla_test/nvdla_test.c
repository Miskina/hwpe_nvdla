#define NVDLA_BASE_ADDR 0x1A130000
#define NVDLA_CDP_D_CYA_0 0xd0b8

#define NVDLA_REG_WRITE(reg_offset, data) *(volatile int *)(NVDLA_BASE_ADDR + reg_offset) = data
#define NVDLA_REG_READ(reg_offset) *(volatile int *)(NVDLA_BASE_ADDR + reg_offset)

void __attribute__((noreturn)) main()
{
	const int a = 1;
	const int b = 2;
	int c = 0;

	c = a + b;

	NVDLA_REG_WRITE(NVDLA_CDP_D_CYA_0, 0xDEADBEEF);
	int reg_val = NVDLA_REG_READ(NVDLA_CDP_D_CYA_0);

	while(1);
}
