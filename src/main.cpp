#include <exception>
#include <boost/log/trivial.hpp>
#include "tun_device.hpp"
#include "irc_vpn.hpp"
#include "system_error.hpp"

using namespace std;

enum ARGS
{
    ARG_PROGRAM,
    ARG_SERVER,
    ARG_SRC_NICKNAME,
    ARG_SRC_ADDRESS,
    ARG_DST_NICKNAME,
    ARG_DST_ADDRESS,

    ARG_COUNT
};

static void drop_privileges(uid_t uid, gid_t gid) {
    if (getuid() == 0) {
        BOOST_LOG_TRIVIAL(info) << "Dropping root privileges";
        system_call(setgid(gid), "Unable to set gid");
        system_call(setuid(uid), "Unable to set uid");
    }
}

int main(int argc, const char ** const argv)
{
    if (ARG_COUNT != argc) {
        cerr << "Usage: ircvpn [server] [source nickname] [source ip] [dest nickname] [dest ip]" << endl;
        return 1;
    }

    BOOST_LOG_TRIVIAL(info) << "Application started";

    try {
        tun_device tun(argv[ARG_SRC_ADDRESS], argv[ARG_DST_ADDRESS]);
        drop_privileges(1000, 1000);

        irc_vpn app(tun, argv[ARG_SERVER], argv[ARG_SRC_NICKNAME]);
        app.run();

    } catch (system_error & e) {
        if (EINTR == e.get_error_code()) {
            BOOST_LOG_TRIVIAL(info) << "Exiting due to a signal";
        } else {
            BOOST_LOG_TRIVIAL(fatal) << "System Error: " << e.what();
        }
    } catch (exception & e) {
        BOOST_LOG_TRIVIAL(fatal) << "System Error: " << e.what();
    }
}
