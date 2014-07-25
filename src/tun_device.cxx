#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <errno.h>
#include "tun_device.h"
#include "system_error.h"
#include "auto_fd.h"

TunDevice::TunDevice(const char * src_ip, const char * dest_ip) :
    m_fd(system_call(open("/dev/net/tun", O_RDWR), "Unable to open the tunnel device"))
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    int fd = m_fd.get_fd();
    system_call(
        ioctl(fd, TUNSETIFF, static_cast<void *>(&ifr)),
        "Unable to create the tunnel device");

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    AutoFD sock = AutoFD(
        system_call(
            socket(AF_INET, SOCK_DGRAM, 0),
            "Unable to create a socket for the tunnel device"));

    int s = sock.get_fd();

    system_call(
        inet_pton(addr.sin_family, src_ip, &addr.sin_addr),
        "Unable to convert the tunnel device IP address");

    ifr.ifr_addr = *(struct sockaddr *)(&addr);
    system_call(
        ioctl(s, SIOCSIFADDR, (caddr_t)(&ifr)),
        "Unable to set the IP address for the tun device");

    system_call(
        inet_pton(addr.sin_family, dest_ip, &addr.sin_addr),
        "Unable to convert the tunnel device IP address");

    ifr.ifr_dstaddr = *(struct sockaddr *)(&addr);
    system_call(
        ioctl(s, SIOCSIFDSTADDR, (caddr_t)(&ifr)),
        "Unable to set the IP address for the tun device");

    ifr.ifr_flags |= IFF_UP;
    system_call(
        ioctl(s, SIOCSIFFLAGS, (caddr_t)(&ifr)),
        "Unable to bring up the tun device");
}
