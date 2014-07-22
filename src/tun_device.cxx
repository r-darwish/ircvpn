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

TunDevice::TunDevice(const char * src_ip, const char * dest_ip) {
    struct ifreq ifr = {0};
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    m_fd = open("/dev/net/tun", O_RDWR);
    if (0 > m_fd) {
        throw SystemError(errno, "Unable to open the tunnel device");
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    if (-1 == ioctl(m_fd, TUNSETIFF, static_cast<void *>(&ifr))) {
        throw SystemError(errno, "IOCTL failed");
    }

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > s) {
        throw SystemError(errno, "Unable to create a socket for the tunnel device");
    }
    AutoFD autofd(s);

    int stat(0);
    stat = inet_pton(addr.sin_family, src_ip, &addr.sin_addr);
    if (1 != stat) {
        throw SystemError(errno, "Unable to convert the tunnel device IP address");
    }

    ifr.ifr_addr = *(struct sockaddr *)(&addr);
    if (-1 == ioctl(s, SIOCSIFADDR, (caddr_t)(&ifr))) {
        throw SystemError(errno, "Unable to set the IP address for the tun device");
    }

    stat = inet_pton(addr.sin_family, dest_ip, &addr.sin_addr);
    if (1 != stat) {
        throw SystemError(errno, "Unable to convert the tunnel device IP address");
    }

    ifr.ifr_dstaddr = *(struct sockaddr *)(&addr);
    if (-1 == ioctl(s, SIOCSIFDSTADDR, (caddr_t)(&ifr))) {
        throw SystemError(errno, "Unable to set the IP address for the tun device");
    }

    ifr.ifr_flags |= IFF_UP;
    if (-1 == ioctl(s, SIOCSIFFLAGS, (caddr_t)(&ifr))) {
        throw SystemError(errno, "Unable to bring up the tun device");
    }

}

TunDevice::~TunDevice() {
    if (0 <= m_fd) {
        close(m_fd);
    }
}