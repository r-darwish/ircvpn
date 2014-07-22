#include <stdio.h>
#include <exception>
#include <signal.h>
#include <errno.h>
#include "tun_device.h"
#include "system_error.h"

using namespace std;

static bool g_continue = true;

static void sig_handler(int signum)
{
    printf("Caught signal %d\n", signum);
    g_continue = false;
}

static void install_sighandler(int signum, sighandler_t handler)
{
    struct sigaction int_handler = {0};
    int_handler.sa_handler = handler;
    if (-1 == sigaction(signum, &int_handler, 0)) {
        throw SystemError(errno, "Unable to set a signal");
    }
}

int main()
{
    try {
        install_sighandler(SIGTERM, sig_handler);
        install_sighandler(SIGQUIT, sig_handler);
        install_sighandler(SIGINT, sig_handler);

        TunDevice tun("10.0.0.1", "10.0.0.2");
        int tun_fd = tun.get_fd();

        while (g_continue) {
            char buffer[4096] = {0};
            ssize_t read_bytes = read(tun_fd, buffer, sizeof(buffer));
            if (-1 == read_bytes) {
                throw SystemError(errno, "Unable to read from the tunnel");
            }
            printf("Read %d\n", read_bytes);
        }
    } catch (SystemError & e) {
        if (EINTR == e.m_errno) {
            printf("Exiting due to signal\n");
        } else {
            printf("Critical Exception: %s\n", e.what());
        }
    } catch (exception & e) {
        printf("Critical Exception: %s\n", e.what());
    }
}
