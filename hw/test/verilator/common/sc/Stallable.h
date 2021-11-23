#ifndef STALLABLE_H
#define STALLABLE_H

#include <systemc.h>

struct Stallable
{
    sc_event stall_finish_e;
    sc_inout<bool> stall;

    void handle_stall()
    {
        if(stall.read())
        {
            wait(stall_finish_e);
            stall.write(0);
        }
    }
};

#endif // STALLABLE_H