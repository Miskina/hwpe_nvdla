#include <stdio.h>
#include <pulp.h>
#include <stdint.h>
#include "../include/nvdla_registers.h"
#include <hal/nvdla/nvdla.h>

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

  printf("%s - Polling read data from addr: %0X\n", test_name, reg_addr);

  int i;
  int result;
  uint32_t read_data;

  for (i = 0; i < 10; ++i)
  {
    read_data = nvdla_read(reg_addr);
    printf("%s - Read data:%0X from addr:%0X\n", test_name, read_data, reg_addr);
    if (read_data == write_data)
    {
      result = 1;
      break;
    }
  }

  if (result)
  {
    printf("%s - Test successful!", test_name);
  }
  else
  {
    printf("%s - Test failed!", test_name);
  }

  return 0;
}
