#include <exception>
#include <boost/log/trivial.hpp>
#include "tun_device.hpp"
#include "irc_vpn.hpp"
#include "system_error.hpp"

using namespace std;

static void drop_privileges(uid_t uid, gid_t gid) {
    if (getuid() == 0) {
        BOOST_LOG_TRIVIAL(info) << "Dropping root privileges";
        system_call(setgid(gid), "Unable to set gid");
        system_call(setuid(uid), "Unable to set uid");
    }
}

int main()
{
    BOOST_LOG_TRIVIAL(info) << "Application started";

    try {
        tun_device tun("10.0.0.1", "10.0.0.2");
        drop_privileges(1000, 1000);

        irc_vpn app(&tun);
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
