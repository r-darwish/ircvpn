#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <sys/signal.h>
#include "irc_vpn.hpp"

using namespace boost::asio;

void irc_vpn::on_quit_signal(
    const boost::system::error_code & error,
    int signal_number)
{
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Signal Error";
    } else {
        BOOST_LOG_TRIVIAL(info) << "Caught signal " << signal_number;
        io.stop();
    }
}

void irc_vpn::on_tun_read(
    const boost::system::error_code & error,
    std::size_t bytes_read)
{
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Error reading from the tunnel";
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Read " << bytes_read << " from the tunnel";
        tun_sd.async_read_some(
            buffer(tun_read_buffer, sizeof(tun_read_buffer)),
            boost::bind(&irc_vpn::on_tun_read, this, _1, _2));
    }
}

void irc_vpn::run()
{
    signal_set quit_signals(io, SIGINT, SIGTERM, SIGQUIT);

    quit_signals.async_wait(
        boost::bind(&irc_vpn::on_quit_signal, this, _1, _2));

    tun_sd.async_read_some(
        buffer(tun_read_buffer, sizeof(tun_read_buffer)),
        boost::bind(&irc_vpn::on_tun_read, this, _1, _2));

    io.run();
}
