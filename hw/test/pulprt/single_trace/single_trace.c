#include <stdio.h>
#include <pulp.h>

#include <nvdla_trace_tests.h>
#include <nvdla.h>

int main()
{
  
  if(TRACE_SETUP_REGISTERS())
  {
    TRACE_LOG("Registers setup success!");
  }
  else
  {
    TRACE_LOG("Registers setup failed!");
    return 1;
  }


  if (TRACE_FINISH())
  {
    TRACE_LOG("Trace finished successfully!\n");
  }
  else
  {
    TRACE_LOG("Trace failed!\n");
    return 2;
  }

  return 0;
}
