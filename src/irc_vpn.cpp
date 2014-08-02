#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <b64/decode.h>
#include <b64/encode.h>
#include <sys/signal.h>
#include "irc_vpn.hpp"

using namespace boost::asio;

static const char MAGIC[] = {'I', 'V', 'P', 'N'};

void irc_vpn::on_private_message(
    const std::string & from, const std::string & message)
{
    base64::decoder decoder;
    char decoded_message[message.length()];
    int decoded_size(decoder.decode(
        message.c_str(), static_cast<int>(message.length()), decoded_message));

    if (0 == decoded_size) {
        BOOST_LOG_TRIVIAL(debug) << "Not a base64 encoded message";
        return;
    }

    if ((static_cast<size_t>(decoded_size) < sizeof(MAGIC)) || (0 != memcmp(MAGIC, decoded_message, sizeof(MAGIC)))) {
        BOOST_LOG_TRIVIAL(debug) << "Packet does not contain the magic";
        return;
    }

    size_t packet_size(static_cast<size_t>(decoded_size) - sizeof(MAGIC));
    BOOST_LOG_TRIVIAL(debug) << "Writing " << packet_size << " bytes to the tunnel";
    async_write(
        tun_sd,
        buffer(decoded_message + sizeof(MAGIC), packet_size),
        boost::bind(&irc_vpn::on_tun_write, this, _1, _2));
}

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

void irc_vpn::on_tun_write(
    const boost::system::error_code & error,
    std::size_t bytes_written)
{
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Error writing to the tunnel: " << error.message();
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Successfully wrote " << bytes_written << " bytes to the tunnel";
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

        size_t packet_size(sizeof(MAGIC) + bytes_read);
        char packet_buffer[packet_size];
        memcpy(packet_buffer, MAGIC, sizeof(MAGIC));
        memcpy(packet_buffer + sizeof(MAGIC), tun_read_buffer, bytes_read);

        size_t encoded_packet_size(packet_size * 3 + 1);
        char encoded_packet[encoded_packet_size];
        encoded_packet[encoded_packet_size - 1] = '\0';
        base64::encoder encoder;

        int encoded_bytes(
            encoder.encode(
                packet_buffer, static_cast<int>(sizeof(packet_buffer)),
                encoded_packet));

        encoder.encode_end(encoded_packet + encoded_bytes);

        std::string string_packet(encoded_packet);
        boost::algorithm::erase_all(string_packet, "\n");
        irc.send_private_message(dest_nickname, string_packet);

        tun_sd.async_read_some(
            buffer(tun_read_buffer, sizeof(tun_read_buffer)),
            boost::bind(&irc_vpn::on_tun_read, this, _1, _2));
    }
}

void irc_vpn::run()
{
    signal_set quit_signals(io, SIGINT, SIGTERM, SIGQUIT);
    irc.on_private_message.connect(
        boost::bind(&irc_vpn::on_private_message, this, _1, _2));

    quit_signals.async_wait(
        boost::bind(&irc_vpn::on_quit_signal, this, _1, _2));

    tun_sd.async_read_some(
        buffer(tun_read_buffer, sizeof(tun_read_buffer)),
        boost::bind(&irc_vpn::on_tun_read, this, _1, _2));

    irc.connect();
    io.run();
}
