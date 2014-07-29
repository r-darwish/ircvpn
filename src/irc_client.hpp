#ifndef __IRC_CLIENT_H__
#define __IRC_CLIENT_H__

#include <boost/signals2.hpp>

class irc_client
{
public:
    irc_client(
        boost::asio::io_service & io,
        const std::string & server,
        const std::string & nickname) :
        io(io),
        irc_socket(io),
        server(server),
        nickname(nickname) { }

    void connect();

    boost::signals2::signal
        <void (const std::string &, const std::string &)>
        on_private_message;

    void send_private_message(
        const std::string & to, const std::string message);


private:
    irc_client(const irc_client & other);
    boost::asio::io_service & io;
    boost::asio::ip::tcp::socket irc_socket;
    boost::asio::streambuf read_buffer;
    std::string server;
    std::string nickname;
    void write_handler(
        const boost::system::error_code & error,
        std::size_t bytes_written);
    void send_data(const std::ostringstream & message);
    void handle_private_message(const std::string & message);
    void handle_server_message(const std::string & message);
    void handle_ping(const std::string & message);
    void handle_message(const std::string & message);
    void on_line_read(
        boost::system::error_code error);
};

#endif
