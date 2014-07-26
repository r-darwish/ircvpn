#include <stdio.h>
#include <exception>
#include <signal.h>
#include <errno.h>
#include <boost/log/trivial.hpp>
#include "tun_device.h"
#include "system_error.h"

using namespace std;

static bool g_continue = true;

static void sig_handler(int signum)
{
    g_continue = false;
}

static void install_sighandler(int signum, sighandler_t handler)
{
    struct sigaction int_handler = {0};
    int_handler.sa_handler = handler;
    if (-1 == sigaction(signum, &int_handler, 0)) {
        throw system_error(errno, "Unable to set a signal");
    }
}

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
        install_sighandler(SIGTERM, sig_handler);
        install_sighandler(SIGQUIT, sig_handler);
        install_sighandler(SIGINT, sig_handler);

        tun_device tun("10.0.0.1", "10.0.0.2");
        int tun_fd = tun.get_fd();
        drop_privileges(1000, 1000);

        while (g_continue) {
            char buffer[4096] = {0};
            ssize_t read_bytes = read(tun_fd, buffer, sizeof(buffer));
            if (-1 == read_bytes) {
                throw system_error(errno, "Unable to read from the tunnel");
            }
            BOOST_LOG_TRIVIAL(debug) << "Read " << read_bytes << " bytes from the tunnel";
        }
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
