#ifndef __TUN_DEVICE_H__
#define __TUN_DEVICE_H__

#include "auto_fd.hpp"

class tun_device
{
public:
    tun_device(const char * src_ip, const char * dest_ip);
    inline int get_fd() const { return this->tun.get_fd(); }

private:
    auto_fd tun;
};

#endif
