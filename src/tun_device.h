#ifndef __TUN_DEVICE_H__
#define __TUN_DEVICE_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include "auto_fd.h"

class TunDevice
{
public:
    TunDevice(const char * src_ip, const char * dest_ip);
    int get_fd() { return this->m_fd.get_fd(); }

private:
    AutoFD m_fd;
};

#endif
