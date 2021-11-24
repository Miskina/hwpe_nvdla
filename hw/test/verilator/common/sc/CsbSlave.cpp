#include "sc/CsbSlave.h"


void CsbSlave::slave_work()
{
        // Initial begin
    ready = 0;
    r_valid = 0;
    r_data = 0;
    wr_complete = 0;

    while(true)
    {
            // On posedge
        wait();

        ready.write(0);
        r_valid.write(0);
        r_data.write(0);
        wr_complete.write(0);

        handle_stall();

        ready.write(1);
        if(request())
        {
            if (write_request())
            {
                printf("[%ull ns] %s - Serving write Request: addr=%x, wdat=%x, nposted=%x\n",
                        sc_time_stamp(),
                        name(),
                        addr.read(),
                        wdat.read(),
                        nposted.read());

                if(nposted.read())
                {

                    printf("[%ull ns] %s - Sending write response\n", sc_time_stamp(), name());
                    wr_complete.write(1);
                }

            }
            else
            {
                printf("[%ull ns] %s - Serving Read request", sc_time_stamp(), name());
                r_data.write(0xDEADBEEF);
                r_valid.write(1);
            }
        }
    }
}


bool CsbSlave::request() const
{
    return valid.read();
}

bool CsbSlave::write_request() const
{
    return write.read();
}