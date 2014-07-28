#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "irc_client.hpp"

using boost::asio::ip::tcp;
using namespace boost::asio;

static const std::string NICK_MESSAGE("NOTICE AUTH :*** Looking up your hostname");

void irc_client::write_handler(
    const boost::system::error_code & error,
    std::size_t bytes_written)
{
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "IRC Write Error: " << error.message();
        connect();
        return;
    }

    BOOST_LOG_TRIVIAL(debug) << "Successfully written " << bytes_written << " bytes";
}

void irc_client::send_data(const std::ostringstream & message_stream)
{
    std::string message(message_stream.str());
    async_write(
        irc_socket,
        buffer(message),
        boost::bind(&irc_client::write_handler, this, _1, _2));
}

void irc_client::handle_server_message(const std::string & message)
{
    if (std::string::npos != message.find(NICK_MESSAGE)) {
        std::ostringstream answer;
        answer << "NICK " << nickname << "\r\n";
        answer << "USER " << nickname << " one two three :four\r\n";
        send_data(answer);
    }
}

void irc_client::handle_ping(const std::string & message)
{
    size_t sep_pos(message.find(':', 0));
    if (std::string::npos == sep_pos) {
        BOOST_LOG_TRIVIAL(error) << "Invalid PING message: " << message;
        return;
    }

    std::string hostname = message.substr(sep_pos + 1);
    std::ostringstream answer;
    answer << "PONG :" << hostname << "\r\n";
    send_data(answer);
}

void irc_client::handle_message(const std::string & message)
{
    if (':' == message[0]) {
        size_t sep_pos(message.find(' ', 0));
        if (std::string::npos == sep_pos) {
            BOOST_LOG_TRIVIAL(error) << "Invalid server message: " << message;
            return;
        }

        std::string server_message = message.substr(
            sep_pos + 1);
        handle_server_message(server_message);
    } else if (boost::starts_with(message, "NOTICE")) {
        handle_server_message(message);
    } else if (boost::starts_with(message, "PING")) {
        handle_ping(message);
    }
}

void irc_client::on_line_read(
    boost::system::error_code error)
{
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "IRC Read Error: " << error.message();
        connect();
        return;
    }

    std::istream is(&read_buffer);
    std::string message;
    getline(is, message);
    BOOST_LOG_TRIVIAL(debug) << "IRC Message: " << message;
    handle_message(message);
    async_read_until(
        irc_socket,
        read_buffer,
        "\r\n",
        boost::bind(&irc_client::on_line_read, this, _1));
}

void irc_client::connect()
{
    tcp::resolver resolver(io);
    tcp::resolver::query query(server, "ircd");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    BOOST_LOG_TRIVIAL(info) << "Connecting to " << server;
    boost::asio::connect(irc_socket, endpoint_iterator);
    BOOST_LOG_TRIVIAL(info) << "Connected";

    async_read_until(
        irc_socket,
        read_buffer,
        "\r\n",
        boost::bind(&irc_client::on_line_read, this, _1));
}


