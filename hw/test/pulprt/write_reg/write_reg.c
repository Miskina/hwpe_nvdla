#include <stdio.h>
#include <pulp.h>
#include <stdint.h>
#include <hal/nvdla/nvdla.h>
#include <opendla.h>

#define BUFFER_SIZE 256

uint8_t tx_buffer[BUFFER_SIZE];


int main()
{
  const char test_name[] = "write_reg";

  printf("Test: %s\n", test_name);
  printf("\nTest writing a single configuration register in NVDLA\n");

  uint32_t write_data = 0xDEADBEEF;
  uint32_t reg_addr = NVDLA_CDP_D_CYA_0;
  nvdla_write(reg_addr, write_data);
  printf("%s - Write data:%0X to addr:%0X\n", test_name, write_data, reg_addr);

  uint32_t read_data = nvdla_read(reg_addr);
  printf("%s - Read data:%0X from addr:%0X\n", test_name, read_data, reg_addr);

  if (write_data == read_data)
  {
    printf("%s - Test successful!", test_name);
  }
  else
  {
    printf("%s - Test failed!", test_name);
  }

  return 0;
}
