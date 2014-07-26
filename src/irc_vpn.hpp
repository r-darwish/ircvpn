#ifndef __IRC_VPN_H__
#define __IRC_VPN_H__

#include "tun_device.hpp"

class irc_vpn
{
public:
    irc_vpn(tun_device * tun) :
        tun(tun)
    { }

    void run();
private:
    tun_device * tun;
};

#endif
