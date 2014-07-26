#ifndef __IRC_VPN_H__
#define __IRC_VPN_H__

#include <boost/asio.hpp>
#include "tun_device.hpp"

static const int TUN_READ_BUFFER_SIZE = 4096;

class irc_vpn
{
public:
    irc_vpn(tun_device * tun) :
        tun_sd(io, tun->get_fd())
    { }

    void run();
private:
    boost::asio::io_service io;
    boost::asio::posix::stream_descriptor tun_sd;
    char tun_read_buffer[TUN_READ_BUFFER_SIZE];
    void on_tun_read(
        const boost::system::error_code & error,
        std::size_t bytes_read);
    void on_quit_signal(
        const boost::system::error_code & error,
        int signal_number);
};

#endif
