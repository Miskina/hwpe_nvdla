#ifndef HWPE_NVDLA_CONTROL_INTERFACE_H
#define HWPE_NVDLA_CONTROL_INTERFACE_H

#include "ControlOperation.h"

class ControlInterface
{

public:

    virtual void submit_operation(ControlOperation&& operation, ControlOperationResponse&& response) = 0;

    virtual bool is_ready() = 0;

};


#endif // HWPE_NVDLA_CONTROL_INTERFACE_H